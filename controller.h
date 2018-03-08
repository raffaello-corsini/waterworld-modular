/***************************************************************************
*            controller.h
*
*  These file is used to describe the valve of a watertank.
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

/*
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_
*/

// Including this library to work with strings.
// #include <string>

#include <ariadne.h>

namespace Ariadne {

  // Posso passargli un automa? O meglio passargli solo la variabile?
  HybridIOAutomaton getController(
    // Controller's label.
    String label,
    // Controlled tank's waterlevel.
    RealVariable waterlevel,
    // Parameter hmin, hmax, delta for the analysis.
    RealParameter hmin,
    RealParameter hmax,
    RealParameter delta,
    // Controlled tank's valve
    HybridIOAutomaton valve,
    // This int represents the number of this component.
    int progressive)
    {

      // 1. Automaton
      HybridIOAutomaton controller(label);

      // 2. Registration of the input/output variables
      controller.add_input_var(waterlevel);

      // 3. Registration of the events

      /*
      * Qui arriva il grosso dubbio: come si prendono gli eventi?
      * Devo passarglieli come parametri alla creazione?
      * Posso trovarmeli qua dentro perché il namespace è comune?
      * Idem per tutte le variabili, eventi, locazioni, etc.
      * utilizzate sia da tank che da valvole che da controllori.
      */

      // Prendo gli eventi che arrivano dalla valvola.
      //const std::set< DiscreteEvent >& input_events() const;
      set<DiscreteEvent> events = valve.input_events();

      set<DiscreteEvent>::iterator it = events.begin();

      // La seguente è come leggono in hybrid_io_automaton.cc l'evento dall'iteratore.
      // Io provo a riutilizzarlo tale e quale.
      // DiscreteEvent(input_event_it->name()

      DiscreteEvent close_event = DiscreteEvent(it->name());

      controller.add_output_event(close_event);

      it++;

      DiscreteEvent open_event = DiscreteEvent(it->name());

      controller.add_output_event(open_event);

      // 4. Registration of the locations

      DiscreteLocation rising("rising" + Ariadne::to_string(progressive));
      DiscreteLocation falling("falling" + Ariadne::to_string(progressive));
      controller.new_mode(rising);
      controller.new_mode(falling);

      // 5. Transitions

      RealExpression waterlevel_leq_hmax = waterlevel - hmax - delta; // x <= hmax + delta
      RealExpression waterlevel_geq_hmin = hmin - delta - waterlevel; // x >= hmin - delta
      controller.new_invariant(rising, waterlevel_leq_hmax);
      controller.new_invariant(falling, waterlevel_geq_hmin);
      RealExpression waterlevel_geq_hmax = waterlevel - hmax + delta; // x >= hmax - delta
      RealExpression waterlevel_leq_hmin = hmin + delta - waterlevel; // x <= hmin + delta
      controller.new_unforced_transition(close_event, rising, falling, waterlevel_geq_hmax);
      controller.new_unforced_transition(open_event, falling, rising, waterlevel_leq_hmin);












      return controller;

    }

  }
