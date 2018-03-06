/***************************************************************************
*            system.h
*
*  This file provides the system definition.
*  Specifically, this is watertank system in which a tank with a hole in the
*  bottom receives an input water flow. Such input flow can
*  be modulated between zero and its maximum by controlling a valve. The
*  described controller aims at keeping the water level between an upper threshold
*  and a lower threshold.
*
* 	Questo file contiene la definizione del sistema nelle sue variabili e componenti.
*  Per altre informazioni si rimanda alla relazione allegata al presente progetto.
*
*  Questa è una versione preliminare con 2 flusi d'ingresso, 3 tank e 4 valvole.
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

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_

#include <ariadne.h>
#include "bottom_tank.h"
#include "side_tank.h"
#include "valve.h"
#include "controller.h"

namespace Ariadne {

  HybridIOAutomaton getSystem()
  {

    // Integer that counts the tanks.
    int tank_counter = 0;
    // Integer that counts the valves.
    int valve_counter = 0;
    // Integer that counts the controllers.
    int controller_counter = 0;

    // 0: System variables

    /*
    * Le variabili contenute in questo sistema saranno 7: ogni watertank sarà dotata di un livello
    * dell'acqua e di una valvola superiore con una determinata apertura. Inoltre andremo ad aggiungere
    * un'ulteriore variabile valvola che rappresenterà l'apertura della valvola che è posta alla fine
    * dell'ultima watertank.
    *
    */

    // Creo le variabili per i livelli d'acqua.

    RealVariable waterLevel0("waterLevel0");
    RealVariable waterLevel1("waterLevel1");
    RealVariable waterLevel2("waterLevel2");

    // RealVariable waterlevel_array[3] = { RealVariable abc("abc"),"b","c"};

    // Creo le variabili per l'apertura delle valvole.
    // Ce ne sarà uno in più rispetto a sopra per
    // la valvola d'uscita dell'ultima watertank.
    RealVariable valveLevel0("valveLevel0");
    RealVariable valveLevel1("valveLevel1");
    RealVariable valveLevel2("valveLevel2");
    //RealVariable valveLevelEnd("valveLevelEnd");

    /// Tank automaton

    // 0: Parametri

    /*
    *	Qua vado a creare i parametri d'ingresso e di uscita.
    *  I parametri d'ingresso sono tutti regolati da una valvola,
    *  quindi consisteranno in un determinato intervallo.
    *  Al momento supponiamo che le valvole siano tutte uguali,
    *  quindi che abbiano lo stesso intervallo di apertura/chiusura.
    */

    // Ingresso dell'entrata della watertank #w0, costante.
    RealParameter w0in("w0in",0.05);
    // Ingresso dell'entrata della watertank #w1, costante.
    RealParameter w1in("w1in",0.05);
    // Uscita delle tank.
    // RealParameter tankOutputFlow("tankOutputFlow",0.04);

    // Provo a crearne di diversi per identificare eventuali anomalie.
    RealParameter tankOutputFlow0("tankOutputFlow0",0.02);
    RealParameter tankOutputFlow1("tankOutputFlow1",0.04);
    RealParameter tankOutputFlow2("tankOutputFlow2",0.08);

    // Provo a creare le due side_tank con la mia funzione.
    HybridIOAutomaton side_tank_0 = Ariadne::getSideTank(
      "side_tank_0",
      waterLevel0,
      valveLevel0,
      valveLevel2,
      w0in,
      tankOutputFlow0,
      // This int represents the number of this tank.
      tank_counter
    );

    tank_counter++;

    HybridIOAutomaton side_tank_1 = Ariadne::getSideTank(
      "side_tank_1",
      waterLevel1,
      valveLevel1,
      valveLevel2,
      w1in,
      tankOutputFlow1,
      // This int represents the number of this tank.
      tank_counter
    );

    tank_counter++;

    // Provo a creare una bottom_tank con la mia funzione.
    HybridIOAutomaton real_bottom_tank = Ariadne::getBottomTank(
      "real_bottom_tank",
      waterLevel2,
      waterLevel0,
      waterLevel1,
      valveLevel2,
      tankOutputFlow0,
      tankOutputFlow1,
      tankOutputFlow2,
      // This int represents the number of this tank.
      tank_counter
    );

    // 1. Automaton

    // Se nel modello del tutorial abbiamo solamente una tank, qua ne abbiamo 3.

    HybridIOAutomaton tank0("tank0");
    HybridIOAutomaton tank1("tank1");
    HybridIOAutomaton tank2("tank2");

    // 2. Registration of the input/output variables

    // Vado ad aggiungere le variabili di input per ogni watertank, ovvero l'apertura della valvola.
    tank0.add_input_var(valveLevel0);
    tank1.add_input_var(valveLevel1);
    tank2.add_input_var(valveLevel2);

    // Aggiungo nell'ultima tank come input anche l'apertura dell'ultima valvola.
    //tank2.add_input_var(valveLevelEnd);
    // Non sono sicuro sia corretto ma aggiungo in input sulla tank2 anche
    // waterLevel0 e waterLevel1, che vado ad utilizzare per definirne la dinamica.
    tank2.add_input_var(waterLevel0);
    tank2.add_input_var(waterLevel1);

    // Aggiungo la variabile di output per ogni tank, il suo livello d'acqua.
    tank0.add_output_var(waterLevel0);
    tank1.add_output_var(waterLevel1);
    tank2.add_output_var(waterLevel2);

    // 4. Registration of the locations

    // Creo le locazioni delle tank, una per ciascuna.
    DiscreteLocation flow0("flow0");
    DiscreteLocation flow1("flow1");
    DiscreteLocation flow2("flow2");

    // Vado ora ad aggiungere ciascuna locazione alla rispettiva watertank.
    tank0.new_mode(flow0);
    tank1.new_mode(flow1);
    tank2.new_mode(flow2);

    /// 5. Registration of the dynamics

    /*
    * Vado a modellare la dinamica.
    * Inizio con le prime due tank in alto.
    * Il livello dell'acqua si evolverà:
    * - aggiungendo l'input per l'apertura della valvola superiore;
    * - perdendo l'acqua che è data dall'uscita della tank * apertura della valvola sottostante / 2.
    * Quel "fratto 2" significa che ciò che fluisce dalla valvola sarà metà dalla prima tank e metà dalla seconda.
    */
    tank0.set_dynamics(flow0, waterLevel0, - tankOutputFlow0 *  ( valveLevel2 / 2 ) * waterLevel0 + w0in * valveLevel0);
    tank1.set_dynamics(flow1, waterLevel1, - tankOutputFlow1 *  ( valveLevel2 / 2 ) * waterLevel1 + w1in * valveLevel1);

    /*
    * Nell'ultima tank entrerà ciò che esce dalle tank precedenti mentre uscirà
    * l'outputflow moltiplicato per l'apertura valvola e per il livello (pressione).
    */

    tank2.set_dynamics(flow2, waterLevel2,
      // Ciò che esce.
      - tankOutputFlow2 * waterLevel2 // * valveLevelEnd
      // Ciò che entra dalla tank 0.
      + tankOutputFlow0 *  ( valveLevel2 / 2 ) * waterLevel0
      // Ciò che entra dalla tank 1.
      + tankOutputFlow1 *  ( valveLevel2 / 2 ) * waterLevel1);

      cout << real_bottom_tank << endl;
      cout << tank2 << endl;
      cout << "------------------" << endl;
      cout << side_tank_0 << endl;
      cout << tank0 << endl;
      cout << "------------------" << endl;
      cout << side_tank_1 << endl;
      cout << tank1 << endl;

      // Sarebbe bello fosse implementato un metodo per verificare
      // se due automi sono equivalenti. Magari c'è già ma non so
      // com'è implementato.
      /*
      cout << "++++++++++++++++++" << endl;
      cout << (real_bottom_tank==tank2) << endl;
      */


      /// Valve automaton

      // 0. Parameters

      // Vado ad inizializzare un parametro che rappresenta
      // la costante temporale per l'apertura e la chiusra
      // delle valvole. Ne imposto solo uno perché suppongo
      // che tutte le valvole siano uguali.

      RealParameter T("T",4.0); // Time constant for opening/closing the valve

      // 1. Automaton

      // Creo quattro valvole. Una sopra ad ogni watertank e una
      // che va a chiudere l'uscita dell'ultima in basso.
      HybridIOAutomaton valve0("valve0");
      HybridIOAutomaton valve1("valve1");
      HybridIOAutomaton valve2("valve2");
      //HybridIOAutomaton valve_end("valve_end");

      //HybridIOAutomaton valve("valve");

      // 2. Registration of the input/output variables

      // Vado a registrare le variabile di input e output.
      // L'unico output che avrò sarà quello di uscita.
      valve0.add_output_var(valveLevel0);
      valve1.add_output_var(valveLevel1);
      valve2.add_output_var(valveLevel2);
      //valve_end.add_output_var(valveLevelEnd);

      // 3 Registration of the input/internal events

      // Vado a registrare i vari stati della valvola.
      // Ad ogni valvola potrà succedere di aprirsi, chiudersi o rimanere in stasi.
      // Vado a creare gli eventi per ogni valvola.
      // Valvola #0.
      DiscreteEvent e_open_0("e_open_0");
      DiscreteEvent e_close_0("e_close_0");
      DiscreteEvent e_idle_0("e_idle_0");
      valve0.add_input_event(e_open_0);
      valve0.add_input_event(e_close_0);
      valve0.add_internal_event(e_idle_0);
      // Valvola #1.
      DiscreteEvent e_open_1("e_open_1");
      DiscreteEvent e_close_1("e_close_1");
      DiscreteEvent e_idle_1("e_idle_1");
      valve1.add_input_event(e_open_1);
      valve1.add_input_event(e_close_1);
      valve1.add_internal_event(e_idle_1);
      // Valvola #2.
      DiscreteEvent e_open_2("e_open_2");
      DiscreteEvent e_close_2("e_close_2");
      DiscreteEvent e_idle_2("e_idle_2");
      valve2.add_input_event(e_open_2);
      valve2.add_input_event(e_close_2);
      valve2.add_internal_event(e_idle_2);
      // Valvola #End.
      /*
      DiscreteEvent e_open_end("e_open_end");
      DiscreteEvent e_close_end("e_close_end");
      DiscreteEvent e_idle_end("e_idle_end");
      valve_end.add_input_event(e_open_end);
      valve_end.add_input_event(e_close_end);
      valve_end.add_internal_event(e_idle_end);
      */

      // 4. Registration of the locations

      // Registrazione delle locazioni.
      // Anche qui si ripete per ogni valvola.
      // Valvola #0.
      DiscreteLocation idle_0("idle_0");
      DiscreteLocation opening_0("opening_0");
      DiscreteLocation closing_0("closing_0");
      valve0.new_mode(idle_0);
      valve0.new_mode(opening_0);
      valve0.new_mode(closing_0);
      // Valvola #1.
      DiscreteLocation idle_1("idle_1");
      DiscreteLocation opening_1("opening_1");
      DiscreteLocation closing_1("closing_1");
      valve1.new_mode(idle_1);
      valve1.new_mode(opening_1);
      valve1.new_mode(closing_1);
      // Valvola #2.
      DiscreteLocation idle_2("idle_2");
      DiscreteLocation opening_2("opening_2");
      DiscreteLocation closing_2("closing_2");
      valve2.new_mode(idle_2);
      valve2.new_mode(opening_2);
      valve2.new_mode(closing_2);
      // Valvola End.
      /*
      DiscreteLocation idle_end("idle_end");
      DiscreteLocation opening_end("opening_end");
      DiscreteLocation closing_end("closing_end");
      valve_end.new_mode(idle_end);
      valve_end.new_mode(opening_end);
      valve_end.new_mode(closing_end);
      */

      // 5. Registration of the dynamics for each location
      // Valvola #0.
      valve0.set_dynamics(idle_0, valveLevel0, 0.0);
      valve0.set_dynamics(opening_0, valveLevel0, 1.0/T);
      valve0.set_dynamics(closing_0, valveLevel0, -1.0/T);
      // Valvola #1.
      valve1.set_dynamics(idle_1, valveLevel1, 0.0);
      valve1.set_dynamics(opening_1, valveLevel1, 1.0/T);
      valve1.set_dynamics(closing_1, valveLevel1, -1.0/T);
      // Valvola #2.
      valve2.set_dynamics(idle_2, valveLevel2, 0.0);
      valve2.set_dynamics(opening_2, valveLevel2, 1.0/T);
      valve2.set_dynamics(closing_2, valveLevel2, -1.0/T);
      // Valvola End.
      /*
      valve_end.set_dynamics(idle_end, valveLevelEnd, 0.0);
      valve_end.set_dynamics(opening_end, valveLevelEnd, 1.0/T);
      valve_end.set_dynamics(closing_end, valveLevelEnd, -1.0/T);
      */

      /// 6. Transitions

      // Guards
      // The library assumes that given a guard g, the relation g >= 0 must hold in the current mode to have a transition

      // Devo impostare le guardie per ogni valvola.
      // Le transizioni sono modellate come nel tutorial originale.
      // Valvola #0.
      RealExpression valveLevel0_geq_one = valveLevel0 - 1.0;
      RealExpression valveLevel0_leq_zero = -valveLevel0;
      // Valvola #1.
      RealExpression valveLevel1_geq_one = valveLevel1 - 1.0;
      RealExpression valveLevel1_leq_zero = -valveLevel1;
      //Valvola #2.
      RealExpression valveLevel2_geq_one = valveLevel2 - 1.0;
      RealExpression valveLevel2_leq_zero = -valveLevel2;
      //Valvola End.
      /*
      RealExpression valveLevelEnd_geq_one = valveLevelEnd - 1.0;
      RealExpression valveLevelEnd_leq_zero = -valveLevelEnd;
      */

      // Resets
      // We need to define a reset for each output variable of the automaton.
      // Anche qua vado a replicare per ogni valvola.

      // Valvola #0.
      std::map<RealVariable,RealExpression> rst_valveLevel0_one;
      rst_valveLevel0_one[valveLevel0] = 1.0;
      std::map<RealVariable,RealExpression> rst_valveLevel0_zero;
      rst_valveLevel0_zero[valveLevel0] = 0.0;
      // Valvola #1.
      std::map<RealVariable,RealExpression> rst_valveLevel1_one;
      rst_valveLevel1_one[valveLevel1] = 1.0;
      std::map<RealVariable,RealExpression> rst_valveLevel1_zero;
      rst_valveLevel1_zero[valveLevel1] = 0.0;
      // Valvola #2.
      std::map<RealVariable,RealExpression> rst_valveLevel2_one;
      rst_valveLevel2_one[valveLevel2] = 1.0;
      std::map<RealVariable,RealExpression> rst_valveLevel2_zero;
      rst_valveLevel2_zero[valveLevel2] = 0.0;
      // Valvola #End.
      /*
      std::map<RealVariable,RealExpression> rst_valveLevelEnd_one;
      rst_valveLevelEnd_one[valveLevelEnd] = 1.0;
      std::map<RealVariable,RealExpression> rst_valveLevelEnd_zero;
      rst_valveLevelEnd_zero[valveLevelEnd] = 0.0;
      */

      // Forced transitions: transitions which implicitly have complementary guards and consequently
      // force the transition to be taken immediately

      // Vado a scrivere le transizione urgenti e non per ogni valvola.
      // Valvola #0.
      valve0.new_forced_transition(e_idle_0, opening_0, idle_0, rst_valveLevel0_one, valveLevel0_geq_one);
      valve0.new_forced_transition(e_idle_0, closing_0, idle_0, rst_valveLevel0_zero, valveLevel0_leq_zero);
      valve0.new_unforced_transition(e_open_0, idle_0, opening_0);
      valve0.new_unforced_transition(e_close_0, idle_0, closing_0);
      // Valvola #1.
      valve1.new_forced_transition(e_idle_1, opening_1, idle_1, rst_valveLevel1_one, valveLevel1_geq_one);
      valve1.new_forced_transition(e_idle_1, closing_1, idle_1, rst_valveLevel1_zero, valveLevel1_leq_zero);
      valve1.new_unforced_transition(e_open_1, idle_1, opening_1);
      valve1.new_unforced_transition(e_close_1, idle_1, closing_1);
      // Valvola #2.
      valve2.new_forced_transition(e_idle_2, opening_2, idle_2, rst_valveLevel2_one, valveLevel2_geq_one);
      valve2.new_forced_transition(e_idle_2, closing_2, idle_2, rst_valveLevel2_zero, valveLevel2_leq_zero);
      valve2.new_unforced_transition(e_open_2, idle_2, opening_2);
      valve2.new_unforced_transition(e_close_2, idle_2, closing_2);
      // Valvola #End.
      /*
      valve_end.new_forced_transition(e_idle_end, opening_end, idle_end, rst_valveLevelEnd_one, valveLevelEnd_geq_one);
      valve_end.new_forced_transition(e_idle_end, closing_end, idle_end, rst_valveLevelEnd_zero, valveLevelEnd_leq_zero);
      valve_end.new_unforced_transition(e_open_end, idle_end, opening_end);
      valve_end.new_unforced_transition(e_close_end, idle_end, closing_end);
      */


      // Creo tre valvole con la funzione getValve.

      HybridIOAutomaton external_valve_0 = Ariadne::getValve(
        "external_valve_0",
        // Valve's opening time.
        T,
        // Valve's opening level.
        valveLevel0,
        // This int represents the number of this component.
        valve_counter
      );

      valve_counter++;

      HybridIOAutomaton external_valve_1 = Ariadne::getValve(
        "external_valve_1",
        // Valve's opening time.
        T,
        // Valve's opening level.
        valveLevel1,
        // This int represents the number of this component.
        valve_counter
      );

      valve_counter++;

      HybridIOAutomaton external_valve_2 = Ariadne::getValve(
        "external_valve_2",
        // Valve's opening time.
        T,
        // Valve's opening level.
        valveLevel2,
        // This int represents the number of this component.
        valve_counter
      );

      // Stampo le valvole per vedere se sono uguali quelle di questo file e quelle generate.
      cout << "@@@@@@@@@@@@@@@@@@" << endl;
      cout << external_valve_0 << endl;
      cout << valve0 << endl;
      cout << "------------------" << endl;
      cout << external_valve_1 << endl;
      cout << valve1 << endl;
      cout << "------------------" << endl;
      cout << external_valve_2 << endl;
      cout << valve2 << endl;

      /// Controller automaton

      // 0. Parameters

      // I parametri li tengo uguali per tutte le tank, visto che le suppongo uguali.
      RealParameter hmin("hmin",5.75); // Lower threshold
      RealParameter hmax("hmax",7.75); // Upper threshold
      RealParameter delta("delta",0.1); // Indetermination constant

      // 1. Automaton

      // Imposto un controllore per ogni automa valvola-tank.
      HybridIOAutomaton controller0("controller0");
      HybridIOAutomaton controller1("controller1");
      HybridIOAutomaton controller2("controller2");

      // 2. Registration of the input/output variables

      // Ogni controllore prende in input il livello d'acqua della tank.
      controller0.add_input_var(waterLevel0);
      controller1.add_input_var(waterLevel1);
      controller2.add_input_var(waterLevel2);

      // 3. Registration of the events

      // Da ogni controller escono in output gli eventi di apertura e chiusura.
      // Il controller #2 regola anche la valvola End. Questa verrà aperta
      // e chiusa in maniera speculare alla valvola #2 e attivata nello stesso
      // momento. Ovvero quando l'acqua nella tank2 supera un certo limite
      // la valvola superiore si chiude e quella sottostante si apre.
      // Controller #0.
      controller0.add_output_event(e_open_0);
      controller0.add_output_event(e_close_0);
      // Controller #1.
      controller1.add_output_event(e_open_1);
      controller1.add_output_event(e_close_1);
      // Controller #2.
      controller2.add_output_event(e_open_2);
      controller2.add_output_event(e_close_2);
      //controller2.add_output_event(e_open_end);
      //controller2.add_output_event(e_close_end);

      // 4. Registration of the locations

      // Ad ogni controllore assegno le proprie locazioni di salita e discesa.
      // Controllore #0.
      DiscreteLocation rising0("rising0");
      DiscreteLocation falling0("falling0");
      controller0.new_mode(rising0);
      controller0.new_mode(falling0);
      // Controllore #1.
      DiscreteLocation rising1("rising1");
      DiscreteLocation falling1("falling1");
      controller1.new_mode(rising1);
      controller1.new_mode(falling1);
      // Controllore #2.
      DiscreteLocation rising2("rising2");
      DiscreteLocation falling2("falling2");
      controller2.new_mode(rising2);
      controller2.new_mode(falling2);

      // 5. Transitions

      // Vado per ogni controllore a scrivere le equazioni degli invarianti, asegnarle,
      // scrivere le guardie e le transizioni che compie.
      // Controllore #0.
      RealExpression waterLevel0_leq_hmax = waterLevel0 - hmax - delta; // x <= hmax + delta
      RealExpression waterLevel0_geq_hmin = hmin - delta - waterLevel0; // x >= hmin - delta
      controller0.new_invariant(rising0, waterLevel0_leq_hmax);
      controller0.new_invariant(falling0, waterLevel0_geq_hmin);
      RealExpression waterLevel0_geq_hmax = waterLevel0 - hmax + delta; // x >= hmax - delta
      RealExpression waterLevel0_leq_hmin = hmin + delta - waterLevel0; // x <= hmin + delta
      controller0.new_unforced_transition(e_close_0, rising0, falling0, waterLevel0_geq_hmax);
      controller0.new_unforced_transition(e_open_0, falling0, rising0, waterLevel0_leq_hmin);
      // Controllore #1.
      RealExpression waterLevel1_leq_hmax = waterLevel1 - hmax - delta; // x <= hmax + delta
      RealExpression waterLevel1_geq_hmin = hmin - delta - waterLevel1; // x >= hmin - delta
      controller1.new_invariant(rising1, waterLevel1_leq_hmax);
      controller1.new_invariant(falling1, waterLevel1_geq_hmin);
      RealExpression waterLevel1_geq_hmax = waterLevel1 - hmax + delta; // x >= hmax - delta
      RealExpression waterLevel1_leq_hmin = hmin + delta - waterLevel1; // x <= hmin + delta
      controller1.new_unforced_transition(e_close_1, rising1, falling1, waterLevel1_geq_hmax);
      controller1.new_unforced_transition(e_open_1, falling1, rising1, waterLevel1_leq_hmin);
      // Controllore #2.
      // Valvola #2 (quella superiore).
      RealExpression waterLevel2_leq_hmax = waterLevel2 - hmax - delta; // x <= hmax + delta
      RealExpression waterLevel2_geq_hmin = hmin - delta - waterLevel2; // x >= hmin - delta
      controller2.new_invariant(rising2, waterLevel2_leq_hmax);
      controller2.new_invariant(falling2, waterLevel2_geq_hmin);
      RealExpression waterLevel2_geq_hmax = waterLevel2 - hmax + delta; // x >= hmax - delta
      RealExpression waterLevel2_leq_hmin = hmin + delta - waterLevel2; // x <= hmin + delta
      controller2.new_unforced_transition(e_close_2, rising2, falling2, waterLevel2_geq_hmax);
      controller2.new_unforced_transition(e_open_2, falling2, rising2, waterLevel2_leq_hmin);
      // Valvola End (quella inferiore).
      //controller2.new_unforced_transition(e_open_end, rising2, falling2, waterLevel2_geq_hmax);
      //controller2.new_unforced_transition(e_close_end, falling2, rising2, waterLevel2_leq_hmin);


      /// Composition

      // Compongo la tripletta #0 tank-valvola-controllore.
      //HybridIOAutomaton semi0 = compose("tank0,valve0,controller0",tank0,valve0,controller0,flow0,idle_0,rising0);
      HybridIOAutomaton semi0 = compose("tank0,valve0",tank0,valve0,flow0,idle_0);
      HybridIOAutomaton automaton0 = compose("automaton0",semi0,controller0,DiscreteLocation("flow0,idle_0"),rising0);
      // Compongo la tripletta #1 tank-valvola-controllore.
      HybridIOAutomaton semi1 = compose("tank1,valve1",tank1,valve1,flow1,idle_1);
      HybridIOAutomaton automaton1 = compose("automaton1",semi1,controller1,DiscreteLocation("flow1,idle_1"),rising1);
      // Compongo la tripletta #2 tank-valvola-controllore.
      HybridIOAutomaton semi2 = compose("tank2,valve2",tank2,valve2,flow2,idle_2);
      HybridIOAutomaton automaton2 = compose("automaton2",semi2,controller2,DiscreteLocation("flow2,idle_2"),rising2);
      // Compongo i tre automi.
      HybridIOAutomaton semisystem = compose("semisystem",automaton0,automaton1,DiscreteLocation("flow0,idle_0,rising0"),DiscreteLocation("flow1,idle_1,rising1"));
      HybridIOAutomaton system = compose("system",semisystem,automaton2,DiscreteLocation("flow0,idle_0,rising0,flow1,idle_1,rising1"),DiscreteLocation("flow2,idle_2,rising2"));

      // Non ho creato un controllore per la valvola finale né l'ho connessa
      // all'automa restituito perché devo ancora decidere che politica utilizzare.

      return system;
    }

  }

  #endif /* TUTORIAL_SYSTEM_H_ */
