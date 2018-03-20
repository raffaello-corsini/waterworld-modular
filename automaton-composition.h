/***************************************************************************
*            automaton-composition.h
*
*  These file is used to describe two functions.
*  The most important one is automaton-composition,
*  which compose a number n of automata in a single
*  one.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

/*
#ifndef VALVE_H_
#define VALVE_H_
*/

#include <utility>

#include <ariadne.h>

namespace Ariadne {

  // Composition of all the automata with the initial location given.
  HybridIOAutomaton composition_all_pieces_together(
    std::vector< pair<HybridIOAutomaton,DiscreteLocation> > mainVector){

      /*
      * Creation of automaton returned by the function.
      * It's initialised with the first one of the vector
      * passed as argument.
      */
      HybridIOAutomaton system = std::get<0>(mainVector.at(0));

      /*
      * Creation of the location of the automaton composed until this point.
      * I will use it inside the loop to indicate this as the starting
      * location of the automaton..
      */
      DiscreteLocation location = std::get<1>(mainVector.at(0));

      // Start of the loop to compose with all the other automata.
      for (unsigned int k = 1; k < mainVector.size(); k++){
        system = compose(
          "final_system_" + Ariadne::to_string(k),
          system,std::get<0>(mainVector.at(k)),
          location,
          std::get<1>(mainVector.at(k)));
          /*
          * Update of the location desidered for the automaton
          * composed until this point.
          */
          location = DiscreteLocation(location.name() + "," + std::get<1>(mainVector.at(k)).name());
        }

        return system;
      }

      /*
      * This is a function used to replace de occurences of ", "
      * in the string view of an automata with "\n", in order to
      * make easier the analysis.
      */

      String easy_read_automaton(std::string& str) {
        size_t start_pos = 0;
        while((start_pos = str.find(", ", start_pos)) != std::string::npos) {
          str.replace(start_pos, 2, "\n");
          start_pos += 1;
        }
        return str;
      }

    }
