/***************************************************************************
*            automaton-composition.h
*
*  These file is used to describe two functions.
*  The first is automaton-composition, which compose
*  a number n of automata in a single one.
*  The second function is used to convert the print of the automaton
*  in a more understandable way.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

/*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Library General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
          * Update of the location desidered for
          * the automaton composed until this point.
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
