/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2024 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#ifndef Workflow_h
#define Workflow_h

#include <QList>


class WorkflowStep;

/**
 * Class describing a workflow consisting of workflow steps.
 * This class is meant to be instantiated, not to be derived from.
 *
 * Usage example:
 *
 *   QList<WorkflowStep *> steps;
 *   steps << new AppInitWorkflowStep     ( WF_InitStep      )
 *         << new AppReadDataWorkflowStep ( WF_ReadDataStep  )
 *         << new AppUserInputWorkflowStep( WF_UserInputStep )
 *         << new AppWriteDataWorkflowStep( WF_WriteDataStep );
 *
 *   Workflow * appWorkflow = new Workflow( steps );
 *   ...
 *   ...
 *   delete appWorkflow;
 *
 *
 * Each workflow step should call  _workflow->next() or _workflow->back()
 * when appropriate, e.g. when the user hits a "Next" or "Back" button.
 **/
class Workflow
{
public:

    /**
     * Constructor: Create a worflow with the specified steps.
     *
     * This class takes ownership of the steps and will delete them when
     * needed. For each step, WorkflowStep::setWorkflow() is called to set its
     * parent workflow to this one.
     **/
    Workflow( const QList<WorkflowStep *> & steps );

    /**
     * Destructor. This will delete all added workflow steps.
     **/
    virtual ~Workflow();

    /**
     * Return the current step or 0 if there is none.
     **/
    WorkflowStep * currentStep() const { return _currentStep; }

    /**
     * Return the workflow step with ID 'id', or 0 if there is no such step.
     **/
    WorkflowStep * step( int id ) const;

    /**
     * Go to the next step and add the current step to the steps history.
     * This does nothing if there is no next step.
     **/
    void next();

    /**
     * Go back, i.e. to the previous step in the steps history, and remove that
     * step from the steps history.
     *
     * This does nothing if there is no previous step.
     **/
    void back();

    /**
     * Go to the step with the specfied ID and add the current step to the
     * steps history.
     *
     * This does nothing if there is no step with that ID.
     **/
    void gotoStep( int id );

    /**
     * Go back to the very first step in the workflow and clear the steps
     * history.
     **/
    void restart();

    /**
     * Return 'true' if the current workflow step is the last one in the list,
     * i.e. if a button label "Finish" would be appropriate for the normal
     * "Next" button.
     **/
    bool atLastStep() const;

    /**
     * Return 'true' if the steps history is empty, i.e. if the current
     * workflow step is the first one: This means that the user cannot go back
     * from here, so the "Back" button (if there is any) should be disabled.
     **/
    bool historyEmpty() const { return _history.isEmpty(); }

    /**
     * Clear the steps history.
     **/
    void clearHistory() { _history.clear(); }


protected:

    /**
     * Push a step to the history unless it is 0.
     **/
    void pushHistory( WorkflowStep * step );

    /**
     * Pop the last entry from the history and return it.
     * Return 0 if the history is already empty.
     **/
    WorkflowStep * popHistory();
    
    /**
     * Deactivate the current workflow step (if there is any)
     * and activate 'step' instead.
     **/
    void activate( WorkflowStep * step, bool goingForward = true );


    //
    // Data members
    //

    QList<WorkflowStep *> _steps;       // Takes ownership
    QList<WorkflowStep *> _history;     // no ownership
    WorkflowStep *        _currentStep;
};


/**
 * Abstract base class for a workflow step.
 *
 * Derived classes should create any widgets either in the constructor
 **/
class WorkflowStep
{
protected:

    /**
     * Constructor:
     *
     * Create a workflow step with ID 'id'.
     *
     * The next step is the one with the ID in 'next' or simply the next one in
     * the list if 'next' is -1.
     *
     * It is recommended to do lazy creation of complex member objects like
     * widgets / widget trees that might cause a delay.
     **/
    WorkflowStep( int id, int next = -1 )
        : _id( id )
        ,_next( next )
        {}

public:

    virtual ~WorkflowStep() {}

    /**
     * This is called when this workflow step is becoming the current one.
     * Derived classes should use this to set up and populate any widgets,
     * or to acquire any ressources.
     *
     * Derived classes are required to implement this.
     **/
    virtual void activate( bool goingForward = true ) = 0;

    /**
     * This is called when this workflow step is no longer the current one,
     * just before the next one is activated.
     *
     * Derived classes can use this to release any resources.
     **/
    virtual void deactivate( bool goingForward = true )
        { Q_UNUSED( goingForward ); }

    /**
     * Return this workflow step's own ID.
     **/
    int id() const { return _id; }

    /**
     * Return the ID of the next workflow step, if one is defined, or
     * WF_StepAuto if the next step in the workflow list should be the next.
     *
     * Notice that there is no 'prev()' counterpart: The workflow manager
     * determines this based on what step was actually the previous one.
     **/
    int next() const { return _next; }

    /**
     * Set the parent workflow. This is done implicitly when a new workflow is
     * created with a QList<WorkflowStep *>.
     *
     * This member variable can be used to tell the workflow to go to the next
     * or the previous step if the user clicks a "Next" or "Back" button.
     **/
    void setWorkflow( Workflow * workflow ) { _workflow = workflow; }

protected:

    Workflow * _workflow;
    int _id;
    int _next;
};


#endif // Workflow_h
