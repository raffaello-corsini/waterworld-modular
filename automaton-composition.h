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

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_
*/

// Including this library to work with strings.
// #include <string>

#include <ariadne.h>

namespace Ariadne {

  /*
  * Primo metodo di composizione. Qui ricevo i vettori contenenti
  * tank, valvole e controllori uno per uno, procedendo a comporre gli
  * automi uno dopo l'altro.
  *
  */

  HybridIOAutomaton composition_all_pieces_by_type(
    std::vector<HybridIOAutomaton>& tanks,
    std::vector<HybridIOAutomaton>& valves,
    std::vector<HybridIOAutomaton>& controllers){

      // Creo un intero progressivo per etichettare il sistema.
      int progressive = 0;


      // Creo l'automa che restituirò alla fine.
      // Lo inizializzo con la prima tank.
      HybridIOAutomaton system = tanks.at(0);


      // Parto componendo tutte le tank rimanenti.
      for (int k = 1; k < tanks.size(); k++){
        // Salvo come carattere il numero di tank a cui sono arrivato.
        String k_char = Ariadne::to_string(k);
        // Converto l'intero progressivo in carattere.
        String prog_char = Ariadne::to_string(progressive);
        // Vado ad estrarmi la locazione della tank, che so essere una.
        // DiscreteLocation loc = tanks.at(k).modes().front().location();
        system = compose(
          "system_1a_" + prog_char,
          system,tanks.at(k),
          system.modes().front().location(),
          // Se fosse un automa qualsiasi userei il metodo seguente
          // commentato per estrarmi la locazione.
          // tanks.at(k).modes().front().location()
          // Ma visto che so che è una tank so quale locazione voglio.
          "flow" + k_char
        );
        progressive++;
      }
      //HybridIOAutomaton semimodule0 = compose("tank0,valve0",tanks.at(0),valves.at(0),DiscreteLocation("flow0"),DiscreteLocation("idle_0"));

      // Compongo tutte le valvole.
      for (int k = 0; k < valves.size(); k++){
        // Salvo come carattere il numero di valvola a cui sono arrivato.
        String k_char = Ariadne::to_string(k);
        // Converto l'intero progressivo in carattere.
        String prog_char = Ariadne::to_string(progressive);
        // Vado ad estrarmi la locazione della tank, che so essere una.
        // DiscreteLocation loc = tanks.at(k).modes().front().location();
        system = compose(
          "system_1b_" + prog_char,
          system,valves.at(k),
          system.modes().front().location(),
          // Se fosse un automa qualsiasi userei il metodo seguente
          // commentato per estrarmi la locazione.
          // tanks.at(k).modes().front().location()
          // Ma visto che so che è una tank so quale locazione voglio.
          "idle_" + k_char
        );
        progressive++;
      }

      // Compongo tutti i controllori.
      for (int k = 0; k < controllers.size(); k++){
        // Salvo come carattere il numero di controllore.
        String k_char = Ariadne::to_string(k);
        // Converto l'intero progressivo in carattere.
        String prog_char = Ariadne::to_string(progressive);
        // Vado ad estrarmi la locazione della tank, che so essere una.
        // DiscreteLocation loc = tanks.at(k).modes().front().location();
        system = compose(
          "system_1c_" + prog_char,
          system,controllers.at(k),
          system.modes().front().location(),
          // Se fosse un automa qualsiasi userei il metodo seguente
          // commentato per estrarmi la locazione.
          // tanks.at(k).modes().front().location()
          // Ma visto che so che è una tank so quale locazione voglio.
          "rising" + k_char
        );
        progressive++;

      }

      return system;
    }

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
            automata.at(k).modes().front().location());
          }

          return system;
        }

        /*
        * Composizione in ordine per tripletta tank-valvola-controllore
        * Qui tank_number == valve_number == controller_number
        * Creo il vettore dove salvo gli automi parziali per poi alla fine
        * comporre tutti i sottoautomi risultanti.
        */
        HybridIOAutomaton composition_of_subsystems(
          std::vector<HybridIOAutomaton>& tanks,
          std::vector<HybridIOAutomaton>& valves,
          std::vector<HybridIOAutomaton>& controllers){

            // Creo un vettore che conterrà gli automi parziali.
            std::vector<HybridIOAutomaton> modules;
            // Composizione delle tank_number triplette.
            for (int k = 0; k < tanks.size(); k++){

              // Converto k in String.
              String num = Ariadne::to_string(k);

              // Compongo la tripletta modulare #k.
              HybridIOAutomaton semimodule = compose(
                "tank" + num +",valve" + num,
                tanks.at(k),
                valves.at(k),
                DiscreteLocation("flow" + num),
                DiscreteLocation("idle_" + num));

                HybridIOAutomaton module = compose(
                  "module_3_" + num,
                  semimodule,
                  controllers.at(k),
                  DiscreteLocation("flow" + num + ",idle_" + num),
                  DiscreteLocation("rising" + num));

                  modules.push_back(module);

                }

                // L'automa che restituirò alla fine,
                // che parte dal primo sottosistema.
                HybridIOAutomaton system = modules.at(0);

                // Composizione dei sottosistemi risultanti.
                // Prendo ogni sottosistema e lo compongo
                // con il risultato delle composizioni precedenti.
                for (int k = 1; k < modules.size(); k++){
                  system = compose(
                    "system_3_" + Ariadne::to_string(k),
                    system,modules.at(k),
                    system.modes().front().location(),
                    modules.at(k).modes().front().location()
                  );

                }

                return system;

              }

              // A function to replace the occurrences of ", " in the automa
              // print with "\n".
              String easy_read_automaton(std::string& str) {
                String from = ", ";
                String to = "\n";
                //if(from.empty())
                //return;
                size_t start_pos = 0;
                while((start_pos = str.find(from, start_pos)) != std::string::npos) {
                  str.replace(start_pos, from.length(), to);
                  start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
                }
                return str;
              }

            }
