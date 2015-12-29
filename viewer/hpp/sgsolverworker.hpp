#ifndef SGSOLVERWORKER_HPP
#define SGSOLVERWORKER_HPP

#include <QtWidgets>
#include "sg.hpp"
#include "sgapprox.hpp"

//! Class for implementing the twist algorithm within SGViewer
/*! If we just used the SGSolver class to solve the game, the
  algorithm would run continuously until convergence or an
  error. Since we want regular status updates and we want to print
  progress to the log window, we have to reimplement the solver
  routine. 

  This class uses SGApprox to calculate the solution. The main program
  assigns this object to a separate thread to preserve responsiveness
  of the algorithm. After each iteration, control is returned to the
  main process, which prints a status update if a revolution has been
  completed and cancels the algorithm if the cancel flag has been
  thrown. Communication between the SGSolverWorker and SGMainWindow is
  facilitated by signals and slots.

  \ingroup viewer
*/
class SGSolverWorker : public QObject
{
  Q_OBJECT;

private:
  //! An environment object to hold settings.
  const SGEnv & env;
  //! Solution object used by SGApprox.
  SGSolution soln;
  //! The main object for performing calculations
  SGApprox approx;
  //! A pointer to the text edit in which to report progress.
  QTextEdit * logTextEdit;

public:
  //! Code for status at the end of the iteration.
  enum STATUS
    {
      FAILED, /*!< Could not complete the iteration. An error
                 occured. */
      CONVERGED, /*!< Algorithm completed the iteration and has
                    converged. */
      NOTCONVERGED /*!< Algorithm completed the iteration but has not
                      converged. */
    } status; /*!< Current status. */

  //! Constructor
  /*! Initializes the SGApprox object. Waits for instruction to begin
      iteration via the SGSolverWorker::iterate slot. */
  SGSolverWorker(const SGEnv & _env,
		 const SGGame & game,
		 QTextEdit * _logTextEdit):
    env(_env), soln(game), approx(env,game,soln),
    logTextEdit(_logTextEdit)
  {
    approx.initialize();
  } // constructor

  //! Returns the status of the worker
  STATUS getStatus() const { return status; }

public slots:

  //! Iterates. 
  /*! Runs one iteration of the twist algorithm and emits the
      resultReady signal. */
  void iterate()
  {
    try
      {
	// Add the extreme tuples array to soln.
	if (approx.numIterations==0)
	  {
	    for (vector<SGTuple>::const_iterator tuple
	    	   = approx.extremeTuples.begin();
	    	 tuple != approx.extremeTuples.end();
	    	 ++tuple)
	      soln.push_back(*tuple);
	  }
	else
	  soln.push_back(approx.extremeTuples.back());
	
	if (approx.generate() > env.getParam(SGEnv::ERRORTOL)
	    && approx.numIterations < env.getParam(SGEnv::MAXITERATIONS))
	  {
	    status = NOTCONVERGED;
	    emit resultReady(false);
	  }
	else
	  {
	    approx.end();
	    
	    status = CONVERGED;
	    emit resultReady(true);
	  }
      }
    catch (exception & e)
      {
	// Add the extreme tuples array to soln.
	qDebug() << "solve failed" << endl;

	approx.end();
	
	status = FAILED;
	emit resultReady(true);
	
	// emit exceptionCaught();
      }
  } // iterate

  //! Returns the SGSolution object.
  const SGSolution & getSolution() const
  { return soln; }

  //! Returns the SGApprox object.
  const SGApprox & getApprox() const
  { return approx; }
  
signals:
  //! Signal that gets emitted when iteration finishes.
  void resultReady(bool);
  //! Signal that gets emitted when an exception is caught.
  /*! Not currently used. */
  void exceptionCaught();  
};

#endif
