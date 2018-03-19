/***************************************************************************
*            automaton-composition.h
*
*  These file is used to describe different functions to compose
*  an automaton made with
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

/*
#ifndef VALVE_H_
#define VALVE_H_
*/

#include <ariadne.h>

namespace Ariadne {

  /*
  * Secondo modo di composizione.
  * Composizione per tipo di automa (tank, valvole, controllori).
  * MA compressi in un unico vettore in un unico vettore.
  *
  */
  HybridIOAutomaton composition_all_pieces_together(
    std::vector<HybridIOAutomaton>& automata){

      // L'automa che restituirò alla fine.
      // Gli assegno il primo elemento del vettore,
      // supponendolo non vuoto.
      HybridIOAutomaton system = automata.at(0);

      // Vado ad aggiungere alla mano tutti gli altri elementi.
      for (int k = 1; k < automata.size(); k++){
        system = compose(
          "system_2_" + Ariadne::to_string(k),
          system,automata.at(k),
          system.modes().front().location(),
          // Passare un vettore di coppie std::vector<pair<HybridIOAutomaton,DiscreteLocation>>
          // Tengo traccia delle locazioni composte.
          automata.at(k).modes().front().location());
        }

        return system;
      }

      // A function to replace the occurrences of ", " in the automa
      // print with "\n".
      String easy_read_automaton(std::string& str) {
        String from = ", ";
        String to = "\n";
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
          str.replace(start_pos, from.length(), to);
          start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        return str;
      }

    }

    // A function to compose three vectors of HybridIOAutomaton.
    std::vector<HybridIOAutomaton> merge_3_vectors(
      std::vector<HybridIOAutomaton>& first,
      std::vector<HybridIOAutomaton>& second,
      std::vector<HybridIOAutomaton>& third){
        std::vector<HybridIOAutomaton> result;
        result.reserve( first.size() + second.size() + third.size() ); // preallocate memory
        result.insert( result.end(), first.begin(), first.end() );
        result.insert( result.end(), second.begin(), second.end() );
        result.insert( result.end(), third.begin(), third.end() );
        return result;
      }


      // Fare l'analisi con transizioni urgenti.
