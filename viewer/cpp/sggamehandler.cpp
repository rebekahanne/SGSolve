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

#include <QtWidgets>
#include "sggamehandler.hpp"

SGGameHandler::SGGameHandler():
  game(SGGame()), errorTol(1e-8)
{
  deltaEdit = new QLineEdit("0.9");
  deltaEdit->setSizePolicy(QSizePolicy::Preferred,
			   QSizePolicy::Preferred);
  
  numStatesEdit = new QLineEdit("1");
  numStatesEdit->setSizePolicy(QSizePolicy::Preferred,
			       QSizePolicy::Preferred);
  numStatesEdit->setReadOnly(true);
  QPushButton * addStateButton = new QPushButton("+");
  QPushButton * removeStateButton = new QPushButton(" -");
  QPushButton * nextStateButton = new QPushButton("->");
  QPushButton * prevStateButton = new QPushButton("<-");

  addStateButton->setToolTip(tr("Add state"));
  removeStateButton->setToolTip(tr("Delete state"));
  nextStateButton->setToolTip(tr("Next state"));
  prevStateButton->setToolTip(tr("Previous state"));

  QSize buttonSize(50,addStateButton->height());
  addStateButton->resize(buttonSize);
  removeStateButton->resize(buttonSize);
  nextStateButton->resize(buttonSize);
  prevStateButton->resize(buttonSize);

  addStateButton->setMinimumWidth(buttonSize.width());
  removeStateButton->setMinimumWidth(buttonSize.width());
  nextStateButton->setMinimumWidth(buttonSize.width());
  prevStateButton->setMinimumWidth(buttonSize.width());

  addStateButton->setSizePolicy(QSizePolicy::Fixed,
				QSizePolicy::Preferred);
  removeStateButton->setSizePolicy(QSizePolicy::Fixed,
				   QSizePolicy::Preferred);
  nextStateButton->setSizePolicy(QSizePolicy::Fixed,
				 QSizePolicy::Preferred);
  prevStateButton->setSizePolicy(QSizePolicy::Fixed,
				 QSizePolicy::Preferred);

  numActionsEdits = vector<QLineEdit*>(2);
  vector<QPushButton *> addActionButtons = vector<QPushButton*>(2);
  vector<QPushButton *> removeActionButtons = vector<QPushButton*>(2);
  for (int player = 0; player < 2; player++)
    {
      numActionsEdits[player] = new QLineEdit("1");
      numActionsEdits[player]->setReadOnly(true);
      numActionsEdits[player]->setSizePolicy(QSizePolicy::Preferred,
					     QSizePolicy::Preferred);

      addActionButtons[player] = new QPushButton("+");
      removeActionButtons[player] = new QPushButton(" -");

      addActionButtons[player]->resize(buttonSize);
      removeActionButtons[player]->resize(buttonSize);

      addActionButtons[player]->setToolTip(QString("Add action\nfor player ")
					   + QString::number(player));
      removeActionButtons[player]->setToolTip(QString("Delete action\nfor player ")
					   + QString::number(player));

      addActionButtons[player]->setMinimumWidth(buttonSize.width());
      removeActionButtons[player]->setMinimumWidth(buttonSize.width());

      addActionButtons[player]->setSizePolicy(QSizePolicy::Fixed,
					      QSizePolicy::Preferred);
      removeActionButtons[player]->setSizePolicy(QSizePolicy::Fixed,
						 QSizePolicy::Preferred);
    }
  
  errorTolEdit = new QLineEdit(QString("%1").arg(errorTol,0,'E',3));
  errorTolEdit->setSizePolicy(QSizePolicy::Preferred,
			      QSizePolicy::Preferred);
  errorTolEdit->setToolTip(tr("The convergence threshhold"));

  payoffTableView = new SGTableView();
  payoffTableView->setSelectionMode(QAbstractItemView::ContiguousSelection);
  probabilityTableViews = vector<SGTableView*>(1);
  probabilityTableViews[0] = new SGTableView();

  payoffModel = NULL;

  probabilityTableLayout = new QVBoxLayout();
  probabilityTableLayout->addWidget(probabilityTableViews[0]);

  initializeModels();

  currentStateCombo = new QComboBox();
  currentStateCombo->addItem("0");
  currentStateCombo->setSizePolicy(QSizePolicy::Preferred,
				   QSizePolicy::Preferred);

  feasibleCheckBox = new QCheckBox(QString("Only calculate feasible set"));

  // Construct layout
  layout = new QVBoxLayout();
  QHBoxLayout * controlLayout = new QHBoxLayout();
  QFormLayout * leftControlLayout = new QFormLayout();
  QFormLayout * centerControlLayout = new QFormLayout();
  QFormLayout * rightControlLayout = new QFormLayout();
  QHBoxLayout * tableLayout = new QHBoxLayout();
  QVBoxLayout * payoffLayout = new QVBoxLayout();
  QVBoxLayout * probabilityLayout = new QVBoxLayout();
  
  solveButton = new QPushButton(tr("&Solve"));
  solveButton->setSizePolicy(QSizePolicy::Fixed,
			     QSizePolicy::Preferred);
  solveButton->resize(300,solveButton->height());

  cancelButton = new QPushButton(tr("Cancel"));
  cancelButton->setSizePolicy(QSizePolicy::Fixed,
			      QSizePolicy::Preferred);
  cancelButton->resize(300,cancelButton->height());

  // qDebug() << "I got to here!!!" << endl;
  
  QHBoxLayout * deltaLayout = new QHBoxLayout();
  deltaLayout->addWidget(deltaEdit);
  deltaLayout->setSpacing(5);

  QHBoxLayout * currentStateLayout = new QHBoxLayout();
  currentStateLayout->addWidget(currentStateCombo);
  currentStateLayout->addWidget(prevStateButton);
  currentStateLayout->addWidget(nextStateButton);
  currentStateLayout->setSpacing(5);
  
  centerControlLayout->addRow(new QLabel(tr("Discount factor:")),
			      deltaLayout);
  centerControlLayout->addRow(new QLabel(tr("Error tolerance:")),
			      errorTolEdit);
  centerControlLayout->addRow(new QLabel(tr("Current state:")),
			      currentStateLayout);
  // leftControlLayout->addRow(removeStateButton,
  // 			    addStateButton);
  // leftControlLayout->setSpacing(0);

  for (int player = 0; player < 2; player ++)
    {
      QHBoxLayout * numActionsLayout = new QHBoxLayout();
      numActionsLayout->addWidget(numActionsEdits[player]);
      numActionsLayout->addWidget(removeActionButtons[player]);
      numActionsLayout->addWidget(addActionButtons[player]);
      numActionsLayout->setSpacing(5);

      QString numActionsLabel = QString(tr("Player "))
	+QString::number(player+1)
	+QString(tr("'s number of actions ("));
      if (player == 0)
	numActionsLabel += QString(tr("row"));
      else
	numActionsLabel += QString(tr("column"));
      numActionsLabel += QString(tr("):"));
      
      leftControlLayout->addRow(numActionsLabel,
				numActionsLayout);
      
    }

  QHBoxLayout * numStatesLayout = new QHBoxLayout();
  numStatesLayout->addWidget(numStatesEdit);
  numStatesLayout->addWidget(removeStateButton);
  numStatesLayout->addWidget(addStateButton);
  numStatesLayout->setSpacing(5);
  leftControlLayout->addRow(new QLabel(tr("Number of states:")),
			    numStatesLayout);
  leftControlLayout->setSpacing(5);

  
  rightControlLayout->addRow(feasibleCheckBox);
  rightControlLayout->addRow(solveButton);
  rightControlLayout->addRow(cancelButton);
  
  controlLayout->addLayout(leftControlLayout);
  controlLayout->addLayout(centerControlLayout);
  controlLayout->addLayout(rightControlLayout);

  payoffLayout->addWidget(new QLabel(tr("Stage payoffs:")));
  payoffLayout->addWidget(payoffTableView);

  QScrollArea * probabilityScrollArea = new QScrollArea();
  QWidget * probabilityWidget = new QWidget();

  probabilityWidget->setLayout(probabilityTableLayout);
  probabilityScrollArea->setWidget(probabilityWidget);

  probabilityScrollArea->setWidgetResizable(true);
  probabilityScrollArea->setSizePolicy(QSizePolicy::Expanding,
				       QSizePolicy::Expanding);
  probabilityScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  probabilityLayout->addWidget(new QLabel(tr("Transition probabilities:")));
  probabilityLayout->addWidget(probabilityScrollArea);

  tableLayout->addLayout(payoffLayout);
  tableLayout->addLayout(probabilityLayout);

  // resizePayoffTable(0,2,0,2);
  // resizeProbabilityTable(0,2,0,2,0,1);
  
  layout->addLayout(controlLayout);
  layout->addLayout(tableLayout);

  // Connect slots
  connect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(currentStateChanged(int)));
  connect(deltaEdit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(discountFactorChanged(const QString &)));
  connect(errorTolEdit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(errorTolChanged(const QString &)));

  connect(addActionButtons[0],SIGNAL(clicked()),
	  this,SLOT(action1Added()));
  connect(addActionButtons[1],SIGNAL(clicked()),
	  this,SLOT(action2Added()));
  connect(addStateButton,SIGNAL(clicked()),
	  this,SLOT(stateAdded()));

  connect(removeActionButtons[0],SIGNAL(clicked()),
	  this,SLOT(action1Removed()));
  connect(removeActionButtons[1],SIGNAL(clicked()),
	  this,SLOT(action2Removed()));
  connect(removeStateButton,SIGNAL(clicked()),
	  this,SLOT(stateRemoved()));

  connect(nextStateButton,SIGNAL(clicked()),
	  this,SLOT(nextState()));
  connect(prevStateButton,SIGNAL(clicked()),
	  this,SLOT(prevState()));

  connect(feasibleCheckBox,SIGNAL(stateChanged(int)),
	  this,SLOT(setConstrained(int)));

  // qDebug() << "Finished sggamehandler constructor" << endl;

}

SGGameHandler::~SGGameHandler()
{
  if (payoffModel != NULL)
    delete payoffModel;
  for (int state = 0; state < game.getNumStates(); state++)
    delete probabilityModels[state];
}

void SGGameHandler::setGame(const SGGame & _game)
{
  game = _game;

  if (game.getConstrained()[0] && game.getConstrained()[1])
    feasibleCheckBox->setChecked(true);
  else
    feasibleCheckBox->setChecked(false);

  changeNumberOfStates(game.getNumStates());
  deltaEdit->setText(QString::number(game.getDelta()));
  for (int player = 0; player < 2; player++)
    numActionsEdits[player]
      ->setText(QString::number(game.getNumActions()[0][player]));
  
  initializeModels();

  // qDebug() << "Finished setting game" << endl;
}

void SGGameHandler::changeNumberOfStates(int newS)
{
  disconnect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	     this,SLOT(currentStateChanged(int)));

  numStatesEdit->setText(QString::number(game.getNumStates()));
  
  int state = currentStateCombo->count();
  // Remove items larger than the new number of states
  while (state > newS)
    currentStateCombo->removeItem(--state);
  // Add new states if number of states increased
  while (state < newS)
    currentStateCombo->addItem(QString::number(state++));

  currentStateCombo->setCurrentIndex(0);

  connect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(currentStateChanged(int)));
  
} // changeNumberOfStates

void SGGameHandler::initializeModels()
{
  // Create new payoffModel
  delete payoffModel;

  payoffModel = new SGPayoffTableModel(&game,0);
  payoffTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
  payoffTableView->setModel(payoffModel);
  payoffTableView->resizeColumnsToContents();

  // Now create new probabilityModels
  for (int state = probabilityModels.size()-1;
       state >= 0;
       state --)
    delete probabilityModels[state];

  // Clear out probabilityTableLayout
  QLayoutItem * item;
  while (item = probabilityTableLayout->takeAt(0))
    delete item->widget();
  
  probabilityModels.clear();
  probabilityTableViews.clear();

  probabilityModels.reserve(game.getNumStates());
  probabilityTableViews.reserve(game.getNumStates());

  // Add new models/views
  for (int state = 0;
       state < game.getNumStates();
       state++)
    pushBackProbabilityTable(state);  
} // initializeModels


void SGGameHandler::pushBackProbabilityTable(int newState)
{
  probabilityTableViews.push_back(new SGTableView());
  probabilityTableViews.back()
    ->verticalScrollBar()->setDisabled(true);
  probabilityTableViews.back()
    ->horizontalScrollBar()->setDisabled(true);

  probabilityTableLayout->addWidget(new QLabel(QString("State ")
					       +QString::number(newState)
					       +QString(":")) );
  probabilityTableLayout->addWidget(probabilityTableViews.back());

  probabilityModels.push_back(new SGProbabilityTableModel(&game,0,newState));

  probabilityTableViews.back()->setModel(probabilityModels.back());

  probabilityTableViews.back()->resizeColumnsToContents();
} // pushBackProbabilityTable

void SGGameHandler::popBackProbabilityTable()
{
  delete probabilityModels.back();
  probabilityModels.pop_back();

  delete probabilityTableLayout->takeAt(probabilityTableLayout->count()-1)->widget();
  delete probabilityTableLayout->takeAt(probabilityTableLayout->count()-1)->widget();
  probabilityTableViews.pop_back();
} // popBackProbabilityTable

void SGGameHandler::setState(int state)
{
  for (int player = 0; player < 2; player ++)
    numActionsEdits[player]
      ->setText(QString::number(game.getNumActions()[state][player]));
  
  payoffModel->setState(state);
  payoffModel->emitLayoutChanged();
  payoffTableView->resizeColumnsToContents();

  for (int nextState = 0; nextState < game.getNumStates(); nextState++)
    {
      probabilityModels[nextState]->setState(state);
      probabilityModels[nextState]->emitLayoutChanged();

      // sizeHint for the SGTableView objects has changed. Call
      // updateGeometry.
      probabilityTableViews[nextState]->resizeColumnsToContents();
      probabilityTableViews[nextState]->updateGeometry();
    }
  
  disconnect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	     this,SLOT(currentStateChanged(int)));
  currentStateCombo->setCurrentIndex(state);
  connect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(currentStateChanged(int)));
  
} // setState

void SGGameHandler::currentStateChanged(int newS)
{
  setState(newS);
}

void SGGameHandler::nextState()
{
  int state = currentStateCombo->currentIndex();

  if (state < currentStateCombo->count()-1)
    setState(state+1);
}

void SGGameHandler::prevState()
{
  int state = currentStateCombo->currentIndex();

  if (state > 0)
    setState(state-1);
}

void SGGameHandler::discountFactorChanged(const QString & text)
{
  double newDelta = text.toDouble();
  if (newDelta > 0 && newDelta < 1)
    game.setDiscountFactor(newDelta);
}

void SGGameHandler::errorTolChanged(const QString & text)
{
  double newErrorTol = text.toDouble();
  if (newErrorTol > 0 && newErrorTol < 1)
    errorTol = newErrorTol;
}

void SGGameHandler::action1Added()
{
  actionAdded(0);
}

void SGGameHandler::action2Added()
{
  actionAdded(1);
}

void SGGameHandler::actionAdded(int player)
{
  if (player < 0 || player > 1)
    return;
  
  int state = currentStateCombo->currentIndex();
  int newAction = game.getNumActions()[state][player];
  
  if (payoffTableView->selectionModel()->hasSelection())
    {
      if (player==1)
	newAction = (payoffTableView->selectionModel()
		     ->selectedIndexes().front().column()+1);
      else
	newAction = (payoffTableView->selectionModel()
		     ->selectedIndexes().front().row()+1);
    }
    
  game.addAction(state,player,newAction);
  numActionsEdits[player]
    ->setText(QString::number(game.getNumActions()[state][player]));

  payoffModel->emitLayoutChanged();
  payoffTableView->resizeColumnToContents(newAction);
  // payoffTableView->updateGeometry();
  for (int nextState = 0; nextState < game.getNumStates(); nextState++)
    {
      probabilityModels[nextState]->emitLayoutChanged();
      probabilityTableViews[nextState]->resizeColumnToContents(newAction);
      probabilityTableViews[nextState]->updateGeometry();
    }
}  // actionAdded

void SGGameHandler::stateAdded()
{
  int newState = currentStateCombo->currentIndex()+1;
  game.addState(newState);
  changeNumberOfStates(game.getNumStates());
  numStatesEdit->setText(QString::number(game.getNumStates()));
  pushBackProbabilityTable(game.getNumStates()-1);
  
  setState(newState);
} // stateAdded

void SGGameHandler::action1Removed()
{
  actionRemoved(0);
}

void SGGameHandler::action2Removed()
{
  actionRemoved(1);
}

void SGGameHandler::actionRemoved(int player)
{
  int state = currentStateCombo->currentIndex();
  if (game.getNumActions()[state][player] == 1)
    return;
  
  int action = game.getNumActions()[state][player]-1;
  if (payoffTableView->selectionModel()->hasSelection())
    {
      if (player == 1)
	action = (payoffTableView->selectionModel()
		  ->selectedIndexes().front().column());
      else
	action = (payoffTableView->selectionModel()
		  ->selectedIndexes().front().row());
    }

  game.removeAction(state,player,action);
  numActionsEdits[player]->setText(QString::number(game.getNumActions()
						   [state][player]));

  payoffModel->emitLayoutChanged();
  for (int statep = 0; statep < game.getNumStates(); statep++)
    {
      probabilityModels[statep]->emitLayoutChanged();
      probabilityTableViews[statep]->updateGeometry();
    }
}

void SGGameHandler::stateRemoved()
{
  if (game.getNumStates() == 1)
    return;

  int state = currentStateCombo->currentIndex();

  popBackProbabilityTable();

  int newState;
  if (state > 0)
    newState = state-1;
  else
    newState = 0;

  disconnect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	     this,SLOT(currentStateChanged(int)));
  currentStateCombo->setCurrentIndex(min(state,newState));
  currentStateCombo->removeItem(currentStateCombo->count()-1);
  connect(currentStateCombo,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(currentStateChanged(int)));
  
  game.removeState(state);

  numStatesEdit->setText(QString::number(game.getNumStates()));

  setState(newState);
  
} // stateRemoved

void SGGameHandler::setConstrained(int newState)
{
  if (feasibleCheckBox->isChecked())
    game.setConstrained(vector<bool>(2,true));
  else
    game.setConstrained(vector<bool>(2,false));
}

