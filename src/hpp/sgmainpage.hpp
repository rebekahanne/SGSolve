// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2016 Benjamin A. Brooks
// 
// SGSolve free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SGSolve is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
// 
// Benjamin A. Brooks
// ben@benjaminbrooks.net
// Chicago, IL

/*!
  \mainpage SGSolve Documentation

  \section introsec Introduction

  SGSolve is a software package for representing, solving, and
  analyzing the equilibria of infinitely repeated stochastic games
  with two players, perfect monitoring, and a public randomization
  device. The library implements an algorithm for computing the
  subgame perfect equilibrium payoff correspondence that was invented
  by Dilip Abreu, Ben Brooks, and Yuliy Sannikov (Abreu, Brooks, and
  Sannikov, 2016). 

  The package consists of two main components. The first piece is a
  library of C++ classes for representing and solving stochastic
  games, and the second is a graphical user interface (SGViewer) for
  using and interacting with the solver library. The purpose of this
  guide is to give advanced users an overview of how the library and
  graphical interface are structured. 

  SGSolve makes use of external software packages: the Boost libraries
  are used for serialalization of data relating to stochastic games,
  which are represented with the SGGame class and are saved as .sgm
  files, and the solutions of stochastic games that are generated by
  the algorithmm, which are represented with the SGSolution and
  SGIteration classes and are saved as .sln files. The graphical
  interface uses the Qt framework and the Qt plotting library
  QCustomPlot. 

  \section installsec Installation

  To use this software, you can directly download the SGViewer
  binaries, which are precompiled for Linux and OS X. The source code
  is also available at www.benjaminbrooks.net/research.shtml. The code
  has been compiled on: CentOS 7 using gcc version 4.8.3 and Boost
  1.53; and on OS X using LLVM version 7.0.2 and Boost 1.57. In both
  cases, the SGViewer was compiled using Qt 5.5. The class SGJYCSolver
  uses Gurobi for linear programming, and the code was compiled with
  Gurobi 5.63 on Linux and Gurobi 6.50 on OS X. To compile the code
  yourself, you need to change the relevant variables in src/makefile
  and examples/makefile to direct the compiler to Boost. If you wish
  to compile the examples using Gurobi, you also need to change the
  directories in grb.mk. You can then build the desired examples by
  calling "make" in either the src or examples directories. After you
  build the source, you can build SGViewer program from the viewer
  directory by first calling "qmake" and then "make".

  \section solversec Overview of the solver library

  The machinery underlying the SGSolve package is a library of
  routines for specifying and solving stochastic games. For examples
  of how to use the library, see risksharing.cpp and
  risksharing.hpp. At its core, this library consists of two main
  classes: SGGame, which is used to specify a game, and SGSolver,
  whose method SGSolver::solve runs the algorithm of Abreu, Brooks,
  and Sannikov (2016).

  An SGGame can be constructed in one of two ways. First, the user can
  specify payoffs and transition probabilities as arrays and pass them
  to the SGGame constructor. Alternatively, the user can create a
  class that derives from SGAbstractGame, and pass an object of the
  derived class to the constructor for SGGame. SGAbstractGame contains
  virtual methods for retrieving the payoffs and transition
  probabilities that can be defined by the user. For an example of how
  to derive from SGAbstractGame, see risksharing.hpp.

  Once the game is constructed, the user can construct an SGSolver for
  that game. SGSolver itself is a relatively simple class whose main
  function is to construct an object of the SGApprox class. This class
  represents an approximation of the equilibrium payoff
  correspondence, consisting of pivot payoffs, a current direction,
  threat payoffs, extreme binding continuation values,
  etc. SGSolver::solve simply constructs an SGApprox and then
  repeatedly invokes SGApprox::generate until the distance between
  successive approximations is below the error threshhold.

  In addition to an SGGame, the SGSolver constructor accepts an
  argument of type SGEnv. The SGEnv class manages parameters for the
  algorithm, such as the threshold for convergence and the maximum
  number of iterations.

  SGSolve saves the results of the computation in an SGSolution
  object. The SGGame and SGSolution classes contain serialization
  methods for saving and restoring games and solutions. SGSolution
  contains a copy of the game that was solved, a list of tuples of
  payoffs that represent the trajectory of the pivot over the course
  of the algorithm, and a list of SGIteration objects that contain
  detailed information about the progress of the algorithm.

  For a more detailed description of the solver library and its usage,
  see \ref sourcepage.

  \section viewersec Overview of the graphical interface

  In addition to using the object model directly, the user can also
  interact with the SGSolve library through the SGViewer graphical
  interface. This interface was constructed using the Qt framework and
  the QCustomPlot plotting library (http://www.qcustomplot.com/). 

  The interface consists of three tabs. The "game tab" is for viewing
  and specifying a stochastic game. It contains tables that display,
  for one state at a time, the players' payoffs and the transition
  probabilities for each pair of actions. The user can edit payoffs,
  probabilities, and the discount factor, as well as add and delete
  actions and states. 

  From the game tab, the user can invoke the solve routine. The
  progress of the algorithm is displayed on the "log tab". Every time
  the pivot completes a revolution, the algorithm will output a status
  update.

  Once the algorithm finishes, the output is displayed in the
  "solution tab". On the right-hand side of this tab is a series of
  plots that display payoffs state-by-state. On the left-hand side is
  a detail plot that decomposes how payoffs are generated. 

  The solution tab can be used in two modes. The mode is selected
  using a combo box at the top of the tab. By default, the solution
  tab runs in "progress" mode, in which it displays the trajectory of
  the pivot over the course of the algorithm. The user can control the
  range of iterations using slider bars underneath the plots. In
  "final" mode, the tab only displays the final estimate of the
  equilibrium payoff correspondence. 

  Right-clicking on a plot brings up additional options for the
  user. The first option, "inspect point", will show how a given
  payoff is decomposed into flow payoffs and continuation
  utilities. The second option, "simulate", brings up another window
  for forward simulating the equilibrium that generates the given
  payoff.
    
  For more details about the object model underlying SGViewer, see the
  \ref viewerpage.

  \section examplesec Examples

  For the benefit of the user, we have included several examples of
  how to use the SGSolve package. The file pd.cpp is an example of a
  two-state game, where the stage game in each state takes the form of
  a prisoner's dilemma. This is the example in Table 1 of Abreu,
  Brooks, and Sannikov (2016). Thsi file shows how to construct an
  SGGame by specifying the payoffs and transition probabilities as
  arrays. A second example in abreusannikov.cpp generates and solves a
  one state game from Abreu and Sannikov (2014).

  The third example is risksharing.cpp and risksharing.hpp. These
  files construct a risk sharing game a la Kocherlakota (1996), in
  which the two players have stochastic endowments and concave
  utility, and can insure one another against income shocks with
  transfers. The file risksharing.hpp constructs a risk sharing game
  by deriving from the SGAbstractGame class, and risksharing.cpp uses
  that class to solve for a variety of parameter values. These files
  generate the analysis for Section 6 of Abreu, Brooks, and Sannikov
  (2016).

  The final example is abs_jyc.cpp, which compares the algorithm of
  Abreu, Brooks, and Sannikov (2016) to a generalized version of the
  algorithm of Judd, Yeltekin, and Conklin (2003) that was implemented
  using Gurobi.

  \section conclusionsec Final thoughts

  The package has many more features that the user will no doubt
  discover. Within the src folder is a src/MATLAB subfolder, that
  contains tools for interfacing between MATLAB and SGSolve. In
  particular, sgmex.cpp is a mex program that can be used to specify,
  solve, and analyze games from within MATLAB. There is also a matlab
  m-file sgmexbuild.m that was used to build sgmex on Linux and OS X
  with MATLAB R2014a. (Note that building sgmex is a bit tricky since
  the mex file needs to be linked to position independent code, and
  the default Boost libraries obtained through most package managers
  were not compiled with the -fPIC compiler flag. Thus, you may need
  to build the Boost serialization library from the source code.)

  This program would not have been possible without the support of
  numerous groups and the contributions of others, and it is entirely
  fitting and appropriate that their contributions should be
  acknowledged. In particular, this program was developed with and
  incorporates elements of a number of other open source projects,
  including the Qt application framework (www.qt.io), Emmanuel
  Eichhammer's QCustomPlot (www.qcustomplot.com), the Boost libraries
  (www.boost.org), and the GNU project (www.gnu.org). I would also
  like to gratefully acknowledge support from the Becker Friedman
  Institute, the University of Chicago, and the National Science
  Foundation.

  Finally, it should go without saying that this program is a work in
  progress. Feedback, bug reports, and contributions are much
  appreciated.

  Enjoy!

  Ben Brooks
  Chicago, IL
  ben@benjaminbrooks.net
*/

/*! \defgroup src SGSolve library
  
  @brief The main solver library.

  For a detailed description of the SGSolve library, see \ref
  sourcepage.
 */

/*! \defgroup viewer SGViewer graphical interface

  @brief The graphical user interface.

  For a detailed description of the SGViewer object model, see \ref
  viewerpage.
 */

/** \example pd.cpp 
    \example risksharing.cpp
    \example risksharing.hpp
    \example abreusannikov.cpp
    \example abs_jyc.cpp
 */

/*! \page sourcepage The SGSolve library

  @brief A detailed description of the SGSolve library.

  \section srcoverviewsec Overview

  The source module is comprised of a library of classes for
  specifying, solving, and exploring the solutions of two player
  stochastic games with perfect monitoring and public randomization. A
  list of classes is contained here: \ref src. This page provides an
  overview of the object model, with additional details in the
  respective class pages. 

  \section srcgamesec Specifying a stochastic game
  
  The first step in using the SGSolve library is translating the
  abstract formulation of a stochastic game into a format that can be
  understood by the library. This translation is facilitated by the
  SGGame class. SGGame has private data members that store the
  discount factor, the stage game payoffs, and the transition
  probabilities as a function of the state and the pair of
  actions. SGGame has multiple constructors. One way to construct an
  SGGame is to pass the payoffs, transition probabilities, and other
  data as stl containers, in particular std::vectors of
  std::vectors. 

  There are two additional parameters that one can pass to the SGGame
  constructor which deserve special mention. The first is called
  "unconstrained", which is a vector of two bools, one for each
  player, indicating whether or not to impose incentive
  constraints. By default, unconstrained is false for both
  players. This parameter can be used in a couple of different
  ways. If the user wants to compute just the feasible payoffs for the
  game, and not the subgame perfect Nash equilibrium payoffs, then the
  user can specify that neither player is incentive constrained. On
  the other hand, if one of the players can commit to an action for
  reasons outside the model, then only that player's unconstrained
  value can be set to true.

  The second parameter worth special mention is a vector of lists of
  equilibrium actions. By default, the algorithm will compute the
  payoffs in equilibria in which any action profile may be used on the
  equilibrium path. This argument can be used to restrict attention to
  a subset of the equilibria where only the listed action profiles are
  used in equilibrium. For example, in the decentralized insurance
  example in risksharing.cpp, both players are allowed to make
  transfers of their endowment to one another. It is obviously without
  loss of generality, however, to restrict attention to those action
  profiles in which at most one player is making a positive
  transfer. This is accomplished by setting the equilibrium actions
  parameter to list only those action profiles which are used in
  equilibrium. Note that even if the equilibrium action profiles are
  restricted, players are implicitly allowed to deviate to any action
  that is available in the given state.

  SGGame has another constructor, which accepts a reference of type
  SGAbstractGame. An alternative, and potentially more useful way of
  specifying the game, is to derive from the SGAbstractGame class. The
  principle is that it is often easier to specify the <i>rule</i> by
  which payoffs or transition probabilities are generated, rather than
  simply denumerating them for all cases. The SGAbstractGame class has
  a pure virtual method SGAbstractGame::payoffs which returns the
  players' flow utilities for a given state and action pair. When the
  user derives from SGAbstractGame, they provide a definition of this
  method that implicitly defines the payoff rule. An example of how to
  construct an SGGame by deriving from SGAbstractGame is provided in
  risksharing.hpp.

  SGGame objects can be serialized and deserialized through static
  SGGame::save and SGGame::load methods. See risksharing.cpp for an
  example. Serialized SGGame objects can be loaded by SGViewer.

  \section srcsolversec Solving a game

  Once one has specified the game, the next step is to solve that game
  by constructing an SGSolver. In addition to the game, the
  constructor for SGSolver accepts an object of type SGEnv (i.e., an
  SG environment). The SGEnv class manages parameters for the behavior
  of the algorithm. For more details, see its class page. Once the
  SGSolver is constructed, the computation of equilibrium payoffs is
  invoked using SGSolver::solve.

  The SGSolver itself is actually a quite small. Most of the heavy
  lifting for the computation is done in the SGApprox class. One can
  think of this class as describing the state of the computation at a
  given iteration, as specified in Abreu, Brooks, and Sannikov. Thus,
  this class contains as private data members a pivot tuple of
  payoffs, a current direction, current threat payoffs, a list of
  extreme binding continuation values for each action, and the past
  trajectory of the pivot. SGApprox also contains methods for
  computing the next approximation. This includes (i) updating the
  extreme binding continuation values, (ii) computing the shallowest
  direction, and (iii) updating the pivot. These steps are broken up
  into several methods. 

  Within SGApprox, the work of computing the extreme binding
  continuation values at each iteration falls to the SGAction
  class. The SGApprox maintains a list of SGAction objects for each
  state. Each SGAction object is associated with a particular state
  and action pair. The SGAction class contains methods for computing
  minimum incentive compatible continuation values, given a game and a
  list of extreme payoff tuples that describe the current feasible
  set, and for computing the extreme binding continuation
  values. These methods are controlled by parameters in the SGEnv
  object. (SGAction is in fact derived from another class
  SGBaseAction, which has essentially the same data members but lacks
  the SGEnv object and the methods for recomputing payoffs. The reason
  for this was so that the data related to the action can be
  serialized without the environment.)

  The whole process of updating the approximation is managed by the
  method SGApprox::generate, which returns the distance between the
  new approximation and the previous one. SGSolver::solve constructs
  an SGApprox object and iteratively calls the generate method until
  the error tolerance is below the level specified in the SGEnv. The
  data from the computation is stored in an SGSolution object, which
  is a member of the SGSolver. 

  The class SGJYCSolver is an alternate solver routine that implements
  a generalized version of the algorithm of Judd, Yeltekin, and
  Conklin (2003). This method approximates the equilibrium payoff
  correspondence by a series of bounding hyperplanes in a fixed set of
  directions. The algorithm iteratively recomputes this approximation
  by solving a series of linear programming problems. For the linear
  programming portion of the procedure, we have used the commercial
  optimization package Gurobi. For more details, see the class page
  for SGJYCSolver.

  \section srcsolutionsec Using the solution

  The output of the algorithm is stored in an object of type
  SGSolution, which is a member of SGSolver and can be accessed using
  the SGSolver::getSolution method. The SGSolution contains a copy of
  the solved game, the trajectory of the pivot (represented as a list
  of SGTuple objects), and a list of SGIteration objects that describe
  how the algorithm behaved at each iteration. It is possible to
  change settings in the SGEnv object so that no iterations are
  stored, or so that iterations are only stored on the last revolution
  of the pivot. (In this latter case, the algorithm iterates to
  convergence, and the runs one extra revolution storing the
  iterations.)

  The SGIteration objects describe in detail how the pivot at a given
  iteration was generated, and how the next direction and
  corresponding substitution was chosen. In particular, the
  SGIteration contains private data members for an actionTuple and a
  regimeTuple, which correspond to the basic pair described in Abreu,
  Brooks, and Sannikov (2016). The SGIteration also contains an array
  of SGBaseAction objects which indicate the extreme binding
  continuation values available for each action. There is one
  SGBaseAction object for each action pair which can still be
  supported at the current iteration. All of these data members can be
  accessed using the appropriate get methods. See the SGIteration
  class page for details. 

  The class SGSimulator can be used to forward simulate an equilibrium
  starting from the payoffs associated with a given
  SGIteration. SGSimulator simulates for a fixed number of periods and
  tracks the empirical distribution of states, extreme tuples, and
  action pairs. For more details, see the corresponding class page.

  For the user's convenience, SGSolution contains static methods for
  serialization (SGSolution::save) and deserialization
  (SGSolution::load). See risksharing.cpp for an example of how these
  are used. Serialized SGSolution objects can be loaded by SGViewer.

  \section srcfurthertopics Further topics
  
  A brief comment is in order on the style in which the package is
  written. Every attempt was made to compartmentalize functionality
  and protect data from extraneous tampering. For that reason, almost
  all data members are private or protected and can only be accessed
  through public mutator methods.
  
  This library provides the core functionality underling the SGViewer
  program which is described in \ref viewerpage. Please see that
  section of the documentation for a detailed description of the
  SGViewer object model. 

 */

/*! \page viewerpage The SGViewer graphical interface
  @brief An detailed description of the SGViewer graphical interface.
  \section vieweroverviewsec Introduction
  
  The SGViewer module is a graphical interface for specifying,
  solving, and exploring the solutions of stochastic games. The
  interface is written in the Qt framework version 5.5 and it makes
  use of the QCustomPlot library (www.qcustomplot.com). This section
  of the documentation is primarily devoted to a description of the
  object model underlying the program. For a brief description of how
  to use the graphical interface, please see \ref viewersec.
  
  The graphical interface is initialized by the SGMainWindow
  class. This class constructs the rest of the interface and handles
  the high level functions that have generalized effects on the
  program: loading and saving games and solutions, solving games, and
  keyboard commands. The layout has three tabs: these are the "game
  tab", the "solution tab", and the "log tab". The game tab is for
  specifying and viewing stochastic games, and the solution tab is for
  exploring the solution of stochastic games. The log tab's function
  is primarily for displaying the progress of the algorithm during
  computation. Each of the game and solution tabs has a separate class
  associated with handling the functionality of that tab. 

  \section viewergametabsec The game tab

  The game tab is managed by an object of the SGGameHandler class. The
  game handler contains a copy of a game, and handles the interface
  between various tables and controls for editing payoffs and
  transition probabilities. Basically, the game tab displays the
  payoff matrix and transiiton probabilities for one state at a
  time. This state is selected using controls at the top of the
  tab. 

  The editing of payoffs and probabilities is implemented using Qt's
  model-view framework. The tables themselves are of the type
  SGTableView derived from QTableView. Each table has a model
  associated with it. All models are derived from SGTableModel, which
  is derived from QAbstractTableModel. SGTableModel adds private
  members: a pointer to an associated SGGame object and an int "state"
  member, which is the state that is currently being edited. For
  payoff tables, the model is SGPayoffTableModel, which adds methods
  for generating header data to indicate action profiles and also
  defines setData/getData methods for interfacing with the SGGame
  object. For probability tables, the model is
  SGProbabilityTableModel, which derives from SGPayoffTableModel. This
  class adds a new data member, which is the tomorrow's state, and
  redefines setData and getData to access the relevant probability
  data in SGGame. When the current state is changed by the user,
  SGGameHandler simply updates the state parameters of all of the
  table models and sends out signals to update the displayed
  data. 

  The game tab also has controls for changing the numbers of actions
  and states. When these options are selected, SGGameHandler simply
  invokves the corresponding method in the SGGame class.
  
  Finally, the game tab has a "Solve" push button which triggers the
  solve routine, and a "Cancel" push button for interrupting the
  computation. More on this in the next subsection.

  \section viewersolvesec Solving a game

  When the user presses the "Solve" button on the game tab, the signal
  is handled by the SGMainWindow class which begins a computation
  using the algorithm. This computation is handled via an intermediary
  class called SGSolverWorker. To start the algorithm, the main window
  constructs an SGSolverWorker and moves it to a new thread so as not
  to freeze the program while the computation progresses. The worker
  constructs an SGApprox object for the given game. The main window
  and the worker communicate back and forth to manage the progress of
  the algorithm. The main window invokes the iterate slot in the
  worker to call SGApprox::generate to run a single iteration of the
  algorithm. When this iteration finishes, the worker signals back to
  the main window that the iteration has finished. SGMainWindow then
  prints a status update to the log tab and depending on the outcome
  of the iteration and the status of the program either terminates the
  computation or signals to the worker to begin another iteration. The
  computation will end if either the algorithm has converged, an error
  occurred in SGApprox::generate, or if the user pressed cancel (in
  which case a cancel flag is set that is observed by the main
  window. The main window and the worker communicate back and forth in
  this manner until the computation terminates, at which point the
  SGSolution object generated by the computation is copied to
  SGSolutionHandler and the worker is destroyed.

  \section viewersolutionsec Interacting with the solution

  When a solution is loaded through SGMainWindow or when one is
  produced by solving a game, it is passed to SGSolutionHandler, which
  is the class that controls the solution tab. This tab contains
  various plots for visualizing the computations performed by the
  algorithm and the final solution of the game. 

  On the right-hand side are a series of SGCustomPlot objects, which
  are derived from QCustomPlot, and whose purpose is to simultaneously
  view payoffs across different states. On the left-hand side is
  another SGCustomPlot for providing a larger and more detailed view
  of payoffs in a single state. SGCustomPlot adds several kinds of
  functionality to QCustomPlot. The most important is adding the
  ability for the user to right click on a point in the plot to bring
  up a context menu which has options for inspecting a particular
  point and for forward simulating the equilibrium that generates a
  particular payoff vector. See the class page for more details on
  SGCustomPlot. 
  
  The primary function of the SGSolutionHandler is to handle the
  plotting of the data from its SGSolution member. The way in which
  the solution is plotted depends on a number of parameters that are
  controlled by the user through various widgets. These widgets
  control the plots indirectly through an SGPlotController
  object. This object aggregates all of the settings in the widgets
  into one set of parameters that tell SGSolutionHandler how to
  plot. In particular, when one of the controls is changed, the
  corresponding signal is connected to a slot in SGPlotController that
  updates the parameter value, and then signals to SGSolutionHandler
  to replot.
  
  SGPlotController maintains pointers to the controlling
  widgets. There are two controls that deserve special mention. There
  are two combo boxes that allow the user to select a particular state
  and action pair to display. The user can use these combos to plot
  the test directions that are generated by a particular action pair
  at a given iteration. Only those action pairs are listed that can
  still be incentivized at the current iteration. The list of states
  is controlled by the class SGStateComboModel, and the list of action
  pairs is controlled by an SGActionComboModel.

  The main method for plotting data from the solution is
  SGSolutionHandler::plot. Plotting is in fact broken up into two
  overloaded versions of this method, one of which plots basic
  features, such as the trajectory of the pivot and the title of the
  plot, whereas the other plots the detailed features on the left-hand
  display. These methods' operation depends on a mode which is
  selected from a combo box at the top of the tab, and whose value is
  stored in SGPlotController. When the mode is set to "Progress", the
  tab will plot the sequence of iterations between a user defined
  "start" and "end". For the current iteration, the program will plot
  in the left-hand SGCustomPlot the test directions which are
  generated by a given action pair. By default, the action pair is
  initialized to the one that generates the next direction. The start
  and end iterations are controlled through sliders at the bottom of
  the tab.

  In "Final" mode, only the final revolution of the pivot will be
  plotted. In this mode, the start slider is disabled, and the end
  slider can be used to select a particular iteration to decompose. In
  either mode, the user can select a particular payoff to display in
  detail by right-clicking on that payoff and selecting "Inspect
  point" from the context menu.
  
  \section viewerotherfeatures Other features of the viewer

  There are two other features of SGViewer which we will mention. The
  behavior of the algorithm depends on a number of parameters. These
  parameters can be controlled using the SGSettingsHandler widget,
  which appears as a pop-up when the user selects Tools->Settings. 

  In addition, the SGSolve library has the ability to forward simulate
  the equilibrium that generates a particular payoff using the
  SGSimulator class. Simulations are run using the SGSimulationHandler
  widget, which can be constructed by the user by right-clicking on
  the payoffs of interest in any of the plots on the solution tab, and
  selecting "Simulate equilibrium" from the resulting context menu.
  
 */
