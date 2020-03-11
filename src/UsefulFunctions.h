//
//  main.cpp
//  FunctionTest
//
//  Created by Daito Manabe on 2020/03/10.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <random>
using namespace std;

template <typename T=int>
const T rnd(const T& max) {
  return rand()%static_cast<int>(max);
};

template <typename T=float>
const T range(const T& value,const T& max,const T& toMin,const T& toMax) {
  return value/max*(toMax-toMin)+toMin;
}

template <typename T=int>
const T rnd(const T& min,const T& max) {
  if (min >= 0 && max <= 1)
    return range<float>(static_cast<float>(rand()%10), 10.f, min, max);
  else
    return rand()%static_cast<int>(max-min)+min;
};

template <typename T=int>
const T rnd_list(const vector<T> mylist) {
  return static_cast<T>(mylist[rand()%mylist.size()]);
}


float zmap(float value, float inputMin, float inputMax, float outputMin,float outputMax, bool clamp = true) {

     if (fabs(inputMin - inputMax) < std::numeric_limits<float>::epsilon()){
          return outputMin;
     } else {
          float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
     
          if( clamp ){
               if(outputMax < outputMin){
                    if( outVal < outputMax )outVal = outputMax;
                    else if( outVal > outputMin )outVal = outputMin;
               }else{
                    if( outVal > outputMax )outVal = outputMax;
                    else if( outVal < outputMin )outVal = outputMin;
               }
          }
          return outVal;
     }

}


auto rndBunchNote = rnd_list<int>;
auto rndBunchOct = rnd_list<int>;

template <typename T=int>
const vector<T> scramble(vector<T> n) {
     std::random_device seed_gen;
     std::mt19937 engine(seed_gen());
     std::shuffle(n.begin(), n.end(), engine);
     //C++17でdepraced  random_shuffle(n.begin(),n.end());
  return n;
}

auto scrambleDur = scramble<unsigned long>;

bool whenMod(int countTurn, unsigned long step) {
  return step%countTurn == 0;
}



template <typename T=int>
T cycle_step(vector<T> v, int step) {
  return v.at(step%v.size());
}

vector<int> rotR(vector<int>& notes, vector<int> scale) {
  transform(notes.begin(),notes.end(),notes.begin(),[&scale](int note){
    note += 1;
    note %= scale.size();
    
    return note;
  });
  
  return notes;
}

vector<int> rotL(vector<int>& notes, vector<int> scale) {
  transform(notes.begin(),notes.end(),notes.begin(),[&scale](int note){
    note -= 1;
    note %= static_cast<int>(scale.size());
    
    if (note < 0)
      note = static_cast<int>(scale.size()-1);
  
    return note;
  });

  return notes;
}


