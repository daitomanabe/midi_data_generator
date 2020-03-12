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
        
    protected:
        const command_type type;
        std::string name;
    };
        
    namespace detail {
        bool validate(const std::string &command) {
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
        std::string normalize(const std::string &command) {
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
        std::vector<std::string> split_by_space(std::string normalized_command) {
            int first = 0;
            int last = normalized_command.find_first_of(' ');

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
        
        std::shared_ptr<command> make_smart(std::shared_ptr<command> node) {
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
    
    command::ref parse_group(std::vector<std::string>::iterator &it) {
        auto group = command::create(command_type::group);
        auto current = command::create(command_type::group);
        group->add(current);
        while(*it != "]") {
            auto c = *it;
            if(c == "[") {
                auto command = parse_group(++it);
                current->add(std::move(command));
            } else if(c == ",") {
                current = command::create(command_type::group);
                group->add(current);
            } else {
                current->add_note(c);
            }
            ++it;
        }
        return detail::make_smart(group);
    }
    
    command::ref parse(const std::string &command) {
        if(!detail::validate(command)) {
            return {};
        }
        auto &&normalized_command = detail::normalize(command);
        auto &&sequence = detail::split_by_space(std::move(normalized_command));
        auto it = sequence.begin();
        
        auto group = command::create(command_type::group);
        auto current = command::create(command_type::group);
        group->add(current);
        
        while(it != sequence.end()) {
            auto c = *it;
            if(c == "[") {
                auto command = parse_group(++it);
                current->add(std::move(command));
            } else if(c == ",") {
                current = command::create(command_type::group);
                group->add(current);
            } else {
                current->add_note(c);
            }
            ++it;
        }
        return detail::make_smart(group);
    }
    
    void print(std::ostream &os, const command::ref command, std::string indent) {
        if(command->is_note()) {
            os << indent << command->note() << "\n";
        } else {
            os << indent << "[\n";
            for(auto &c : command->children) {
                print(os, c, indent + "  ");
            }
            os << indent << "]\n";
        }
    }
    
    std::ostream &operator<<(std::ostream &os, const command::ref command) {
        if(command->is_note()) {
            os << command->note();
        } else {
            os << "[\n";
            for(auto &c : command->children) {
                print(os, c, "  ");
            }
            os << "]\n";
        }
        
        return os;
    }
}

int main(int argc, char *argv[]) {
    std::string command = "[[bd sd bd, sd], hh hh, ch ch]";
    auto seq = tidaloid::parse(command);
    std::cout << "seq: " << seq << std::endl;
    return 0;
}
