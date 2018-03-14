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
* Questo file contiene la definizione del sistema nelle sue variabili e componenti.
*  Per altre informazioni si rimanda alla relazione allegata al presente progetto.
*
*  Questa è una versione preliminare con 2 flussi d'ingresso, 3 tank e 3 valvole.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_

#include <ariadne.h>
#include "bottom_tank.h"
#include "side_tank.h"
#include "valve.h"
#include "controller.h"
#include "automaton-composition.h"

namespace Ariadne {

  HybridIOAutomaton getSystem() {

    // Integer that counts the tanks.
    int tank_counter = 0;
    // Integer that counts the valves.
    int valve_counter = 0;
    // Integer that counts the controllers.
    int controller_counter = 0;

    // Number of the tanks.
    int tank_number = 3;
    // Number of the valves.
    int valve_number = 3;
    // Number of the controllers.
    int controller_number = 3;

    // 0: System variables

    // Creo le variabili per i livelli d'acqua.

    std::vector<RealVariable> waterlevels;

    for (int k = 0; k < tank_number; k++){
      waterlevels.push_back(RealVariable("waterLevel" + Ariadne::to_string(k)));
    }

    // Creo le variabili per l'apertura delle valvole.

    std::vector<RealVariable> valvelevels;

    for (int k = 0; k < valve_number; k++){
      valvelevels.push_back(RealVariable("valveLevel" + Ariadne::to_string(k)));
    }

    /// Tank automaton

    // 0: Parametri

    // Creo il vettore per i parametri di input delle tank superiori.
    std::vector<RealParameter> upperflows;

    // Creo il vettore per le tank.
    std::vector<HybridIOAutomaton> tanks;

    // Ingresso dell'entrata della watertank #w0, costante.
    upperflows.push_back(RealParameter("w0in",0.5));
    // Ingresso dell'entrata della watertank #w1, costante.
    upperflows.push_back(RealParameter("w1in",0.5));
    // Uscita delle tank.

    // Crep il vettore per i parametri di uscita.
    std::vector<RealParameter> lowerflows;

    lowerflows.push_back(RealParameter("tankOutputFlow0",0.04));
    lowerflows.push_back(RealParameter("tankOutputFlow1",0.04));
    lowerflows.push_back(RealParameter("tankOutputFlow2",0.04));

    for (int k = 0; k < 2; k++){
      // Provo a creare le due side_tank con la mia funzione.
      HybridIOAutomaton local_tank = Ariadne::getSideTank(
        //"side_tank_0",
        waterlevels.at(k),
        valvelevels.at(k),
        valvelevels.at(2),
        upperflows.at(k),
        lowerflows.at(k),
        // This int represents the number of this tank.
        tank_counter
      );
      tanks.push_back(local_tank);
      tank_counter++;
    }

    // Provo a creare una bottom_tank con la mia funzione.
    HybridIOAutomaton real_bottom_tank = Ariadne::getBottomTank(
      waterlevels.at(2),
      waterlevels.at(0),
      waterlevels.at(1),
      valvelevels.at(2),
      lowerflows.at(0),
      lowerflows.at(1),
      lowerflows.at(2),
      // This int represents the number of this tank.
      tank_counter
    );

    tanks.push_back(real_bottom_tank);

    /// Valve automaton

    // 0. Parameters

    RealParameter T("T",4.0); // Time constant for opening/closing the valve

    // 1. Automaton

    std::vector<HybridIOAutomaton> valves;

    // Creo tre valvole con la funzione getValve.

    for (int k = 0; k < valve_number; k++){
      HybridIOAutomaton valve = Ariadne::getValve(
        // Valve's opening time.
        T,
        // Valve's opening level.
        valvelevels.at(k),
        // This int represents the number of this component.
        valve_counter
      );
      valves.push_back(valve);
      valve_counter++;
    }

    /// Controller automaton

    // 0. Parameters

    // I parametri li tengo uguali per tutte le tank, visto che le suppongo uguali.
    RealParameter hmin("hmin",5.75); // Lower threshold
    RealParameter hmax("hmax",7.75); // Upper threshold
    RealParameter delta("delta",0.002); // Indetermination constant

    // 1. Automata

    std::vector<HybridIOAutomaton> controllers;

    // Creo tre valvole con la funzione getValve.

    for (int k = 0; k < controller_number; k++){
      HybridIOAutomaton controller = Ariadne::getController(
        // Controlled tank's waterlevel.
        waterlevels.at(k),
        hmin,hmax,delta,
        // Controlled tank's valve
        valves.at(k),
        // This int represents the number of this component.
        controller_counter
      );
      controllers.push_back(controller);
      controller_counter++;
    }

    // Compongo la tripletta modulare #0.
    HybridIOAutomaton semimodule0 = compose("tank0,valve0",tanks.at(0),valves.at(0),DiscreteLocation("flow0"),DiscreteLocation("idle_0"));
    HybridIOAutomaton module0 = compose("module0",semimodule0,controllers.at(0),DiscreteLocation("flow0,idle_0"),DiscreteLocation("rising0"));
    // Compongo la tripletta modulare #1.
    HybridIOAutomaton semimodule1 = compose("tank1,valve1",tanks.at(1),valves.at(1),DiscreteLocation("flow1"),DiscreteLocation("idle_1"));
    HybridIOAutomaton module1 = compose("module1",semimodule1,controllers.at(1),DiscreteLocation("flow1,idle_1"),DiscreteLocation("rising1"));
    // Compongo la tripletta modulare #0.
    HybridIOAutomaton semimodule2 = compose("tank2,valve2",tanks.at(2),valves.at(2),DiscreteLocation("flow2"),DiscreteLocation("idle_2"));
    HybridIOAutomaton module2 = compose("module2",semimodule2,controllers.at(2),DiscreteLocation("flow2,idle_2"),DiscreteLocation("rising2"));
    // Compongo i tre automi.
    HybridIOAutomaton semimodulesystem = compose("semimodulesystem",module0,module1,DiscreteLocation("flow0,idle_0,rising0"),DiscreteLocation("flow1,idle_1,rising1"));
    HybridIOAutomaton modulesystem = compose("modulesystem",semimodulesystem,module2,DiscreteLocation("flow0,idle_0,rising0,flow1,idle_1,rising1"),DiscreteLocation("flow2,idle_2,rising2"));

    /*
    * Provo ora a comporre gli automi. Visto che l'ordine di composizione a
    * livello teorico è irrilevante dovrei poter farlo unendo i componenti
    * di ogni tipo alla mano. Provo a vederlo se posso aggiungere in maniera
    * continuativa ogni elemento. Magari prima aggiungo ogni componente
    * ad un unico vettore e poi scorro lungo tutto il vettore.
    *
    * Però posso anche comporre ogni tripletta e poi unire il tutto.
    * Soprattutto per questo specifico caso perché si di avere 3-3-3
    * e potrei rimanere fedele e regolare coi nomi.
    */

    // Primo metodo.
    HybridIOAutomaton temp1 = composition_all_pieces_by_type(tanks,valves,controllers);

    // Creo un vettore unico per eseguire il secondo metodo.
    std::vector<HybridIOAutomaton> wholeVector;
    wholeVector.reserve( tanks.size() + valves.size() + controllers.size() ); // preallocate memory
    wholeVector.insert( wholeVector.end(), tanks.begin(), tanks.end() );
    wholeVector.insert( wholeVector.end(), valves.begin(), valves.end() );
    wholeVector.insert( wholeVector.end(), controllers.begin(), controllers.end() );

    // Secondo metodo.
    HybridIOAutomaton temp2 = composition_all_pieces_together(wholeVector);
    // Terzo metodo.
    HybridIOAutomaton temp3 = composition_of_subsystems(tanks,valves,controllers);
    /*
    // Iniziamo con un po' di stampe allegre per testare vari metodi.
    cout << "Questa è la tank 0." << endl << tanks.at(0) << endl;
    cout << "Questa è la funzione state_space()." << endl << tanks.at(0).state_space() << endl;
    cout << "Questi sono i modi." << endl << tanks.at(0).modes() << endl;
    cout << "Questo è il front() della list dei modi." << endl << tanks.at(0).modes().front() << endl;
    cout << "Questo è la location() del front()." << endl << tanks.at(0).modes().front().location() << endl;
    cout << "Questo è la name() della location()." << endl << tanks.at(0).modes().front().location().name() << endl;
    cout << "---------------------------------" << endl;
    // Valvole.
    cout << "Questa è la valvola 0." << endl << valves.at(0) << endl;
    cout << "Questa è la funzione state_space()." << endl << valves.at(0).state_space() << endl;
    cout << "Questi sono i modi." << endl << valves.at(0).modes() << endl;
    cout << "Questo è il front() della list dei modi." << endl << valves.at(0).modes().front() << endl;
    cout << "Questo è la location() del front()." << endl << valves.at(0).modes().front().location() << endl;
    cout << "Questo è la name() della location()." << endl << valves.at(0).modes().front().location().name() << endl;
    cout << "---------------------------------" << endl;
    // Controllore.
    cout << "Questa è il controllore 0." << endl << controllers.at(0) << endl;
    cout << "Questa è la funzione state_space()." << endl << controllers.at(0).state_space() << endl;
    cout << "Questi sono i modi." << endl << controllers.at(0).modes() << endl;
    cout << "Questo è il front() della list dei modi." << endl << controllers.at(0).modes().front() << endl;
    cout << "Questo è la location() del front()." << endl << controllers.at(0).modes().front().location() << endl;
    cout << "Questo è la name() della location()." << endl << controllers.at(0).modes().front().location().name() << endl;
    cout << "---------------------------------" << endl;

    */

    /*

    // Creo un automa vuoto.
    HybridIOAutomaton dummy;

    // Vedo se gli assegno un altro automa.
    dummy = valves.at(0);
    cout << "----------Stampa valvola-----------"  << endl;
    cout << valves.at(0) << endl;
    cout << "----------Stampa dummy-----------"  << endl;
    cout << dummy << endl;

    HybridIOAutomaton pippo = dummy;
    cout << "----------Stampa pippo-----------"  << endl;
    cout << pippo << endl;

    cout << "----------Modifico dummy-----------"  << endl;
    dummy = tanks.at(0);
    cout << "----------Stampa pippo-----------"  << endl;
    cout << pippo << endl;
    cout << "----------Stampa dummy modificato-----------"  << endl;
    cout << dummy << endl;

    // Con queste stampe in sequenza vedo che se modifico una variabile
    // che era stata assegnata a un'altra quest'ultima riferisce ancora all'originale.

    */

    /*
    // Voglio vedere ora se funziona la composizione ricorsiva.
    // Creo una tank fantoccio.
    HybridIOAutomaton fantoccio = Ariadne::getSideTank(
    waterlevels.at(0),
    valvelevels.at(0),
    valvelevels.at(2),
    upperflows.at(0),
    lowerflows.at(0),
    // This int represents the number of this tank.
    5
  );
  //cout << fantoccio << endl;

  //cout << tanks.at(0) << endl;

  // Creo una seconda tank fantoccio.
  HybridIOAutomaton fantoccio2 = Ariadne::getValve(
  // Valve's opening time.
  T,
  // Valve's opening level.
  valvelevels.at(0),
  // This int represents the number of this component.
  6
);
// cout << fantoccio2 << endl;

// cout << valves.at(0) << endl;

// Provo ora la composizione ricorsiva.
HybridIOAutomaton recursive = compose("tank5,valve6",fantoccio,fantoccio2,DiscreteLocation("flow5"),DiscreteLocation("idle_6"));

cout << semimodule0 << endl << "----------------------" << endl << recursive << endl;

fantoccio = compose("tank5,valve6",fantoccio,fantoccio2,DiscreteLocation("flow5"),DiscreteLocation("idle_6"));

cout << "---------------" << endl << fantoccio << endl;

*/

/*

String prova = Ariadne::to_string(semimodule0);

cout << "Stampo prova con salvato semimodule0" << endl << prova << endl;

cout << "---------------" << endl;

easy_read_automaton(prova);

cout << "Stampo prova con gli \", \" convertiti in \"\\n\"." << endl << prova << endl;

cout << "---------------" << endl;

*/

//cout << temp1 << endl << "---" << endl << temp2 << endl << "---" << endl << temp3 << endl;

// Vado a scrivere su file in maniera più leggibile i miei tre metodi.

ofstream myfile;
myfile.open ("metodo0.txt");
String original = Ariadne::to_string(modulesystem);
myfile << easy_read_automaton(original);
myfile.close();
myfile.open ("metodo1.txt");
String a = Ariadne::to_string(temp1);
myfile << easy_read_automaton(a);
myfile.close();
myfile.open ("metodo2.txt");
String b = Ariadne::to_string(temp2);
myfile << easy_read_automaton(b);
myfile.close();
myfile.open ("metodo3.txt");
String c = Ariadne::to_string(temp3);
myfile << easy_read_automaton(c);
myfile.close();

cout << "Alla fine esce che metodo0 = metodo3 e metodo1 = metodo2." << endl;


/*
ofstream myfile;
myfile.open ("newfashionsystem.txt");
myfile << modulesystem;
myfile.close();
*/

// cout << modulesystem << endl;

return modulesystem;

}

}

#endif /* TUTORIAL_SYSTEM_H_ */
