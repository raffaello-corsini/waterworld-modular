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

#include <utility>

#include <ariadne.h>

namespace Ariadne {

  /*
  * Secondo modo di composizione.
  * Composizione per tipo di automa (tank, valvole, controllori).
  * MA compressi in un unico vettore in un unico vettore.
  *
  */
  HybridIOAutomaton composition_all_pieces_together(
    std::vector< pair<HybridIOAutomaton,DiscreteLocation> > mainVector){

      // L'automa che restituirò alla fine.
      // Gli assegno il primo elemento del vettore,
      // supponendolo non vuoto.
      HybridIOAutomaton system = std::get<0>(mainVector.at(0));

      //        std::get<0>(mainVector.at(tank_number + k)),

      // Creo la locazione dell'automa fin qui composto.
      // La userò nel ciclo per indicarla come partenza
      // nella composizione. L'altra locazione arriverà
      // dal vettore di coppie passato.
      // DiscreteLocation location = std::get<1>(mainVector.at(0));

      // Vado ad aggiungere alla mano tutti gli altri elementi.
      for (unsigned int k = 1; k < mainVector.size(); k++){
        system = compose(
          "final_system_" + Ariadne::to_string(k),
          system,std::get<0>(mainVector.at(k)),//automata.at(k),
          system.modes().front().location(),
          // Passare un vettore di coppie std::vector<pair<HybridIOAutomaton,DiscreteLocation>>
          // Tengo traccia delle locazioni composte.
          std::get<1>(mainVector.at(k)));
          // Come prendere la locazione da utilizzare nel tenere traccia?
          // Se io so che ho già un automa che ha come front una locazione regolare
          // posso comunque prendere quella ?
          //location = DiscreteLocation();
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
