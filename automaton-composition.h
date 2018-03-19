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

  // Composizione di tutti gli automi con locazione iniziale quella data.
  HybridIOAutomaton composition_all_pieces_together(
    std::vector< pair<HybridIOAutomaton,DiscreteLocation> > mainVector){

      /*
       * L'automa che restituirò alla fine.
       * Gli assegno il primo elemento del vettore,
       * supponendolo non vuoto.
       */
      HybridIOAutomaton system = std::get<0>(mainVector.at(0));

      /*
       * Creo la locazione dell'automa fin qui composto.
       * La userò nel ciclo per indicarla come partenza
       * nella composizione. L'altra locazione arriverà
       * dal vettore di coppie passato.
       */
      DiscreteLocation location = std::get<1>(mainVector.at(0));

      // Vado ad aggiungere alla mano tutti gli altri elementi.
      for (unsigned int k = 1; k < mainVector.size(); k++){
        system = compose(
          "final_system_" + Ariadne::to_string(k),
          system,std::get<0>(mainVector.at(k)),
          // Composizione delle etichette tenendo traccia,
          // virgola come composizione.
          system.modes().front().location(),
          std::get<1>(mainVector.at(k)));
          // Come prendere la locazione da utilizzare nel tenere traccia?
          // Se io so che ho già un automa che ha come front una locazione regolare
          // posso comunque prendere quella ?
          //location = DiscreteLocation();
        }

        return system;
      }

      // A function to replace the occurrences of ", "
      // in the automa print with "\n".
      String easy_read_automaton(std::string& str) {
        //String from = ", ";
        //String to = "\n";
        size_t start_pos = 0;
        while((start_pos = str.find(", ", start_pos)) != std::string::npos) {
          str.replace(start_pos, 2, "\n");
          start_pos += 1; // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        return str;
      }

    }

    // Fare l'analisi con transizioni urgenti.
