#include "MidiConstants.h"
#include "MidiSetting.h"

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <regex>
#include <exception>

namespace tidaloid {
    enum class command_type : std::uint8_t {
        note,
        group
    };
    
    struct command {
        using ref = std::shared_ptr<command>;
        
        static command::ref create(command_type type)
        { return std::make_shared<command>(type); };
        
        command(command_type type)
        : type{type}
        {};
        
        bool is_note() const
        { return type == command_type::note; };
        bool is_group() const
        { return type == command_type::group; };
        
        void add(command::ref command)
        { children.push_back(std::move(command)); };
        
        void add_layer(command::ref command)
        { layers.push_back(std::move(command)); };
        
        void add_note(std::string note_name) {
            auto note = create(command_type::note);
            note->set_note(note_name);
            children.push_back(std::move(note));
        }
        
        std::string note() const
        { return name; };
        
        void set_note(std::string name)
        { this->name = std::move(name); };
        
        std::vector<command::ref> children;
        std::vector<command::ref> layers;

        void print(std::ostream &os, std::string indent) const {
            if(layers.size()) for(auto &layer : layers) {
                layer->print(os, indent);
            } else {
                if(is_note()) {
                    os << indent << note() << "\n";
                } else {
                    os << indent << "[\n";
                    for(auto &c : children) {
                        c->print(os, indent + "  ");
                    }
                    os << indent << "]\n";
                }
            }
        }
        
        friend std::ostream &operator<<(std::ostream &os, const command::ref command) {
            command->print(os, "");
            return os;
        }

    protected:
        const command_type type;
        std::string name;
    };

    namespace detail {
        inline static bool validate(const std::string &command) {
            auto nest_begin_num = std::count(command.begin(), command.end(), '[');
            auto nest_end_num = std::count(command.begin(), command.end(), ']');
            if(nest_begin_num != nest_end_num) {
                std::cerr << "number of group markers is not matched.";
                return false;
            }
            auto first_nest_begin_pos = std::find(command.begin(), command.end(), '[');
            auto first_nest_end_pos = std::find(command.begin(), command.end(), ']');
            if(std::distance(command.begin(), first_nest_end_pos) < std::distance(command.begin(), first_nest_begin_pos)) {
                std::cerr << "']' is appearanced before '['";
                return false;
            }
            auto last_nest_begin_pos = std::find(command.rbegin(), command.rend(), '[');
            auto last_nest_end_pos = std::find(command.rbegin(), command.rend(), ']');
            if(std::distance(command.rbegin(), last_nest_begin_pos) < std::distance(command.rbegin(), last_nest_end_pos)) {
                std::cerr << "last '[' is appearanced after ']'";
                return false;
            }
            return true;
        }
        inline static std::string normalize(const std::string &command) {
            std::regex special_chars{"[\\[|\\]|\\,]"};
            std::regex spaces{"\\ +"};
            std::regex empty_group{"\\[\\ *\\]"};
            std::regex treat_target{"(^\\ )|(\\ $)"};
            auto redundant = std::regex_replace(command, special_chars, " $& ");
            auto empty_group_removed = std::regex_replace(redundant, empty_group, " ");
            auto not_treated = std::regex_replace(redundant, spaces, " ");
            auto res = std::regex_replace(not_treated, treat_target, "");
            return res;
        }
        inline static std::vector<std::string> split_by_space(std::string normalized_command) {
            auto last = normalized_command.find_first_of(' ');
            decltype(last) first = 0;

            std::vector<std::string> result;

            while(first < normalized_command.size()) {
                std::string sub_str(normalized_command, first, last - first);

                result.push_back(sub_str);

                first = last + 1;
                last = normalized_command.find_first_of(' ', first);

                if (last == std::string::npos) {
                    last = normalized_command.size();
                }
            }
            
            return result;
        }
        
        inline static std::shared_ptr<command> make_smart(std::shared_ptr<command> node) {
            if(!node || node->children.size() == 0) {
                return node;
            } else if(1 < node->children.size()) {
                for(std::size_t i = 0; i < node->children.size(); ++i) {
                    auto child = node->children[i];
                    node->children[i] = make_smart(node->children[i]);
                }
                return node;
            }
            return make_smart(node->children.front());
        }
    };
    
    inline static command::ref parse_group(std::vector<std::string>::iterator &it) {
        auto group = command::create(command_type::group);
        auto current = command::create(command_type::group);
        group->add_layer(current);
        while(*it != "]") {
            auto c = *it;
            if(c == "[") {
                auto command = parse_group(++it);
                current->add(std::move(command));
            } else if(c == ",") {
                current = command::create(command_type::group);
                group->add_layer(current);
            } else {
                current->add_note(c);
            }
            ++it;
        }
        return detail::make_smart(group);
    }
    
    inline static command::ref parse(const std::string &command) {
        if(!detail::validate(command)) {
            return {};
        }
        auto &&normalized_command = detail::normalize(command);
        auto &&sequence = detail::split_by_space(std::move(normalized_command));
        auto it = sequence.begin();
        
        auto group = command::create(command_type::group);
        auto current = command::create(command_type::group);
        group->add_layer(current);
        
        while(it != sequence.end()) {
            auto c = *it;
            if(c == "[") {
                auto command = parse_group(++it);
                current->add(std::move(command));
            } else if(c == ",") {
                current = command::create(command_type::group);
                group->add_layer(current);
            } else {
                current->add_note(c);
            }
            ++it;
        }
        return detail::make_smart(group);
    }
    
    namespace detail {
        inline static void eval_impl(smf::MidiFile &file,
                                     int track_id,
                                     command::ref sequence,
                                     const std::map<std::string, int> &note_table,
                                     int duration_in_ticks = MIDI::ONEMEASURE,
                                     int offset_in_ticks = 0)
        {
            for(const auto &layer : sequence->layers) {
                const auto &children = layer->children;
                const float unit_duration = duration_in_ticks / (float)children.size();
                for(std::size_t i = 0; i < children.size(); ++i) {
                    const auto &c = children[i];
                    int offset = unit_duration * i;
                    if(c->is_note()) {
                        if(note_table.find(c->note()) == note_table.end()) continue;
                        auto note = note_table.at(c->note());
                        file.addNoteOn(track_id,
                                       offset_in_ticks + offset,
                                       0,
                                       note,
                                       100);
                        file.addNoteOff(track_id,
                                        offset_in_ticks + offset + 30,
                                        0,
                                        note);
                    } else {
                        eval_impl(file, track_id, c, note_table, unit_duration, offset);
                    }
                }
            }
        }
    }
    
    inline static void eval(smf::MidiFile &file,
                            int track_id,
                            std::string sequence_str,
                            const std::map<std::string, int> &note_table,
                            int duration_in_ticks = MIDI::ONEMEASURE,
                            int offset_in_ticks = 0)
    {
        auto &&sequence = parse(sequence_str);
        detail::eval_impl(file, track_id, sequence, note_table, duration_in_ticks, offset_in_ticks);
        file.sortTracks();
    }
    
    inline static void eval(smf::MidiFile &file,
                            std::string sequence_str,
                            const std::map<std::string, int> &note_table,
                            MIDI::Setting setting) 
    {
        eval(file, setting.track_id, sequence_str, note_table, setting.duration_in_ticks, setting.offset_in_ticks);
    }
}

