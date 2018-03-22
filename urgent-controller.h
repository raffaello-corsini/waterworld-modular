/***************************************************************************
*            urgent-controller.h
*
*  These file is used to describe the controller of a valve on the top
*  of a watertank. In this version all the transitions are urgent.
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

#include <ariadne.h>

namespace Ariadne {

  HybridIOAutomaton getUrgentController(
    // Controlled tank's waterlevel.
    RealVariable waterlevel,
    // Parameter hmin, hmax for the analysis.
    RealParameter hmin,
    RealParameter hmax,
    // Controlled tank's valve
    HybridIOAutomaton valve,
    // This int represents the number of this component.
    int progressive){

      // Conversion of the progressive integer to a String.
      String number = Ariadne::to_string(progressive);

      // 1. Automaton
      HybridIOAutomaton controller("controller" + number);

      // 2. Registration of the input/output variables
      controller.add_input_var(waterlevel);

      // 3. Registration of the events

      // Extraction of the events from the given valve.
      set<DiscreteEvent> events = valve.input_events();
      set<DiscreteEvent>::iterator it = events.begin();

      // Extraction and store of the single events.
      DiscreteEvent close_event = *it;
      controller.add_output_event(close_event);
      it++;
      DiscreteEvent open_event = *it;
      controller.add_output_event(open_event);

      // 4. Registration of the locations
      DiscreteLocation rising("rising" + number);
      DiscreteLocation falling("falling" + number);
      controller.new_mode(rising);
      controller.new_mode(falling);

      // 5. Transitions
      RealExpression waterlevel_geq_hmax = waterlevel - hmax;
      RealExpression waterlevel_leq_hmin = hmin - waterlevel;
      controller.new_forced_transition(close_event, rising, falling, waterlevel_geq_hmax);
      controller.new_forced_transition(open_event, falling, rising, waterlevel_leq_hmin);

      return controller;

    }

  }
