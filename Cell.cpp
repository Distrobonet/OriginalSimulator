//
// Filename:        "Cell.cpp"
//
// Programmer:      Ross Mead
// Last modified:   13Apr2011
//
// Description:     This class implements a robot cell.
//

// preprocessor directives
#include <stdio.h>
#include "Cell.h"
#include "Environment.h"



// <protected static data members>
GLint Cell::nCells = 0;



// <constructors>

//
// Cell(dx, dy, dz, theta, colorIndex)
// Last modified: 06Nov2009
//
// Default constructor that initializes
// this cell to the parameterized values.
//
// Returns:     <none>
// Parameters:
//      dx          in      the initial x-coordinate of the cell (default 0)
//      dy          in      the initial y-coordinate of the cell (default 0)
//      dz          in      the initial z-coordinate of the cell (default 0)
//      theta       in      the initial heading of the cell (default 0)
//      colorIndex  in      the initial array index of the color of the cell
//
Cell::Cell(const GLfloat dx,    const GLfloat dy, const GLfloat dz,
    const GLfloat theta, const Color   colorIndex)
: State(), Neighborhood(), Robot(dx, dy, dz, theta, colorIndex)
{
  init(dx, dy, dz, theta, colorIndex);
  ID      = nCells++;
  numBids = 0;
}   // Cell(const GLfloat..<4>, const Color)



//
// Cell(c)
// Last modified: 27Aug2006
//
// Copy constructor that copies the contents of
// the parameterized cell into this cell.
//
// Returns:     <none>
// Parameters:
//      c       in/out      the cell being copied
//
Cell::Cell(const Cell &c): State(c), Neighborhood(c), Robot(c)
{
  leftNbr  = c.leftNbr;
  rightNbr = c.rightNbr;
}   // Cell(const Cell &)



// <destructors>

//
// ~Cell()
// Last modified: 27Aug2006
//
// Destructor that clears this cell.
//
// Returns:     <none>
// Parameters:  <none>
//
Cell::~Cell()
{
}   // ~Cell()



// <public mutator functions>

//
// bool setState(s)
// Last modified: 27Aug2006
//
// Attempts to set the state to the parameterized state,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:
//      s       in/out  the state to be set to
//
bool Cell::setState(const State &s)
{
  *this = s;
  return true;
}   // setState(const State &)



//
// bool setNbrs(nh)
// Last modified: 27Aug2006
//
// Attempts to set the neighborhood to the parameterized neighborhood,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:
//      nh      in/out  the neighborhood to be set to
//
bool Cell::setNbrs(Neighborhood &nh)
{
  *this = nh;
  return true;
}   // setNbrs(Neighborhood &)



//
// bool setRobot(r)
// Last modified: 14May2007
//
// Attempts to set the robot to the parameterized robot,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:
//      r       in/out  the robot to be set to
//
bool Cell::setRobot(const Robot &r)
{
  //if(VERBOSE) printf("in setRobot() ============\n");

  //changed to  cast *this as a Robot variable
  (Robot)*this = r;
  //1if(VERBOSE) printf("robot set============\n");
  return true;
}   // setRobot(const Robot &)

bool Cell::setRobotP(Robot *r)
{
  //(Robot *)this = r;
  return true;
}


// <public accessor functions>

//
// State getState() const
// Last modified: 27Aug2006
//
// Returns the state of this cell.
//
// Returns:     the state of this cell
// Parameters:  <none>
//
State Cell::getState() const
{
  return (State)*this;
}   // getState() const



//
// Neighborhood getNbrs() const
// Last modified: 27Aug2006
//
// Returns the neighborhood of this cell.
//
// Returns:     the neighborhood of this cell
// Parameters:  <none>
//
Neighborhood Cell::getNbrs() const
{
  return (Neighborhood)*this;
}   // getNbrs() const



//
// Robot getRobot() const
// Last modified: 27Aug2006
//
// Returns the robot of this cell.
//
// Returns:     the robot of this cell
// Parameters:  <none>
//
Robot Cell::getRobot() const
{
  return (Robot)*this;
}   // getRobot() const



// <virtual public utility functions>

//
// void draw()
// Last modified: 27Aug2006
//
// Renders the cell as a robot.
//
// Returns:     <none>
// Parameters:  <none>
//
void Cell::draw()
{
  Robot::draw();
}   // draw()



//
// void step()
// Last modified: 30Nov2009
//
// Processes packets received and updates the state of the cell,
// which is then broadcast within the neighborhood of the cell.
//
// Returns:     <none>
// Parameters:  <none>
//
Cell* Cell::cStep()
{
  Cell* answer = NULL;
  if (processPackets())
  {
    if (getNNbrs() > 0)
    {
      updateState();
      sendStateToNbrs();
    }
    moveError();
  }
  if(auctionStepCount>0)
  {
    auctionStepCount++;
  }
  if((AUTONOMOUS_INIT)&&(env->getRobots().size()>0))
  {
    if((getNNbrs() < NEIGHBORHOOD_SIZE)&&(bids.size()==0))
    {
      if(((getState().transError.magnitude()>0)||
            (getID()==formation.getSeedID()))&&
          (getState().transError.magnitude() < MAX_TRANSLATIONAL_ERROR))
      {
        if(auctionStepCount==0)
        {
          answer = this;
        }
      }
    }
  }
  if(CELL_INFO_VIEW)
  {
    cout << "============================="<<endl;
    cout << "cell.getID() = " << getID() << endl;
    cout << "cell.getNNbrs() = " << getNNbrs() << endl;
    cout << "cell.rightNbr->getID() = ";
    if(rightNbr != NULL)
    {
      cout << rightNbr->ID << endl;
    }else
    {
      cout << " NULL " << endl;
    }
    cout << "cell.leftNbr->getID() = ";
    if(leftNbr != NULL)
    {
      cout << leftNbr->ID << endl;
    }else
    {
      cout << " NULL " << endl;
    }
    cout << "rotError = " << rotError << endl;
    cout << "transError = " << transError.magnitude() << endl;
    cout << "behavior.getStatus() = " << behavior.getStatus() << endl;
    cout << "seedID = " << formation.getSeedID() << endl;
    cout << "gradient = " << gradient << endl;
    cout << "formationID = " << formation.getFormationID() << endl;
  }
  Robot::step();
  return answer;
}


//
// void updateState()
// Last modified: 30Nov2009
//
// Updates the state of the cell based upon the
// current states of the neighbors of the cell.
//
// Returns:     <none>
// Parameters:  <none>
//

void Cell::updateState()
{
  if ((getNNbrs()               == 0)     ||
      (nbrWithMinStep()->tStep  <  tStep) ||
      ((formation.getSeedID()   != ID)    &&
       (nbrWithMaxStep()->tStep == tStep))) return;

  // update actual relationships to neighbors
  Neighbor *currNbr = NULL;
  for (GLint i = 0; i < size(); ++i)
  {
    currNbr = getNbr(i);
    if (currNbr == NULL) break;

    // change formation if a neighbor has changed formation
    if (currNbr->formation.getFormationID() > formation.getFormationID())
      changeFormation(currNbr->formation, *currNbr);
    currNbr->relActual = getRelationship(currNbr->ID);
  }
  rels = getRelationships();

  // update temperature
  GLfloat m    = 1.0f;  // mass
  //GLfloat C    = 2.11f;  // specific heat
  //GLfloat K    = 2.0f;  // thermal conductivity (of material)
  //GLfloat A    = PI * getRadius();  // area of contact
  GLfloat C    = 1.0f;  // specific heat
  GLfloat K    = 1.0f;  // thermal conductivity (of material)
  GLfloat A    = 1.0f;  // area of contact
  GLfloat dT   = 0.0f;  // difference in temperature
  GLfloat d    = 0.0f;  // distance
  GLfloat q    = 0.0f;  // heat
  GLfloat qSum = 0.0f;  // accumulated heat
  for (GLint i = 0; i < size(); ++i)
  {
	currNbr = getNbr(i);
	if (currNbr == NULL) break;

	dT    = currNbr->temperature - temperature;
	d     = currNbr->relActual.magnitude();
	q     = K * A * dT / d;
	qSum += q;
  }
  // FACTOR IN ROOM TEMPERATURE!!!
  //temperature = ((qSum - heat) + m * C * temperature) / (m * C);
  temperature = (qSum + m * C * temperature) / (m * C);
  heat        = qSum;
  //printf("T[%d] = %.4f\n", ID, temperature);

  // reference the neighbor with the minimum gradient
  // to establish the correct position in formation
  if (getNNbrs() > 0)
  {
    Neighbor     *refNbr     = nbrWithMinGradient(
        formation.getSeedGradient());
    Relationship *nbrRelToMe = relWithID(refNbr->rels, ID);
    if ((formation.getSeedID() != ID)   &&
        (refNbr                != NULL) &&
        (nbrRelToMe            != NULL))
    {

      // error (state) is based upon the
      // accumulated error in the formation
      Vector  nbrRelToMeDesired = nbrRelToMe->relDesired;
      nbrRelToMeDesired.rotateRelative(-refNbr->rotError);
      GLfloat theta = scaleDegrees(nbrRelToMe->relActual.angle() -
          (-refNbr->relActual).angle());
      rotError      = scaleDegrees(theta + refNbr->rotError);
      transError    = nbrRelToMeDesired - nbrRelToMe->relActual +
        refNbr->transError;
      transError.rotateRelative(-theta);
      //set the state variable of refID  = ID of the reference nbr.
      refID = refNbr->ID;
    }
  }

  tStep = max(tStep + 1, nbrWithMaxStep()->tStep);
}   // updateState()


/*void Cell::updateState()
  {
  Neighbor currNbr;
  for (GLint i = 0; i < getNNbrs(); ++i)
  {
  if (!getHead(currNbr)) break;

// change formation if a neighbor has changed formation
if (getNbr(0)->formation.getFormationID() > formation.getFormationID())
changeFormation(getNbr(0)->formation, *getNbr(0));
getNbr(0)->relActual = getRelationship(currNbr.ID);
++(*this);
}
rels = getRelationships();

// reference the neighbor with the smallest gradient
// to establish correct position in formation
Neighbor     *refNbr = nbrWithMinGradient();
Relationship *nbrRel = relWithID(refNbr->rels, ID);
if ((formation.getSeedID() != ID) && (refNbr != NULL) && (nbrRel != NULL))
{

// error (state) is based upon the accumulated error in the formation
nbrRel->relDesired.rotateRelative(-refNbr->rotError);
GLfloat theta = scaleDegrees(nbrRel->relActual.angle() -
(-refNbr->relActual).angle());
rotError      = scaleDegrees(theta + refNbr->rotError);
transError    = nbrRel->relDesired - nbrRel->relActual +
refNbr->transError;
transError.rotateRelative(-theta);
if (transError.norm() > threshold()) moveArc(transError);
else
if (abs(rotError) > angThreshold())
moveArc(0.0, degreesToRadians(-rotError));
//if (abs(scaleDegrees(refNbr->relActual.angle() -
//                    refNbr->relDesired.angle())) > angThreshold())
//orientTo(refNbr->relActual, refNbr->relDesired.angle());
else moveStop();
}
else moveStop();
}   // updateState()*/

/*void Cell::updateState()
  {
  if(VERBOSE) printf("entering updateState()\n");
  Neighbor currNbr;
  for (GLint i = 0; i < getNNbrs(); ++i)
  {
  if (!getHead(currNbr)) break;

// change formation if a neighbor has changed formation
if (getNbr(0)->formation.getFormationID() > formation.getFormationID())
changeFormation(getNbr(0)->formation, *getNbr(0));
getNbr(0)->relActual = getRelationship(currNbr.ID);
++(*this);
}
rels = getRelationships();

// reference the neighbor with the smallest gradient
// to establish correct position in formation
if(VERBOSE) printf("updateState() -- BEFORE nbrWithMinGradient()\n");
Neighbor     *refNbr = nbrWithMinGradient(formation.getSeedGradient());
if(VERBOSE) printf("updateState() -- AFTER nbrWithMinGradient()\n");
Relationship *nbrRel;
if(refNbr == NULL){
//printf("refNbr == NULL\n");
}else{
nbrRel = relWithID(refNbr->rels, ID);


if(VERBOSE) printf("updateState() -- AFTER relWithID()\n");
}
if ((formation.getSeedID() != ID) && (refNbr != NULL) && (nbrRel != NULL))
{

// error (state) is based upon the accumulated error in the formation
nbrRel->relDesired.rotateRelative(-refNbr->rotError);
GLfloat theta = scaleDegrees(nbrRel->relActual.angle() -
(-refNbr->relActual).angle());
rotError      = scaleDegrees(theta + refNbr->rotError);
transError    = nbrRel->relDesired - nbrRel->relActual +
refNbr->transError;
transError.rotateRelative(-theta);
if (transError.norm() > threshold()) moveArc(transError);
else
if (abs(rotError) > angThreshold())
moveArc(0.0, degreesToRadians(-rotError));
//if (abs(scaleDegrees(refNbr->relActual.angle() -
//                   refNbr->relDesired.angle())) > angThreshold())
//orientTo(refNbr->relActual, refNbr->relDesired.angle());
else moveStop();
}
else moveStop();
if(VERBOSE) printf("leaving updateState()\n");
}   // updateState()*/



// <virtual public neighborhood functions>

//
// bool changeFormation()
// Last modified: 30Nov2009
//
// Attempts to change the formation of the cell,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:
//      f       in/out      the formation to change to
//      n       in          the neighbor instigating the formation change
//
bool Cell::changeFormation(const Formation &f, Neighbor n)
{
  formation = f;
  if (formation.getSeedID() == ID)
  {
    gradient   = formation.getSeedGradient();
    transError = Vector();
    rotError   = 0.0f;
  }
  else
  {
    Relationship *nbrRelToMe = relWithID(n.rels, ID);
    if (nbrRelToMe == NULL) return false;
    nbrRelToMe->relDesired.rotateRelative(n.formation.getHeading());
    gradient                 = n.gradient + nbrRelToMe->relDesired;
    transError               = Vector();
    rotError                 = 0.0f;
  }
  vector<Vector> r = formation.getRelationships(gradient);

  /*--ROSS--
    GLfloat currDist        = 0.0f, closestDist = GLfloat(RAND_MAX);
    GLint   closestNbrIndex = -1;
  //GLint   closestRelIndex = -1;
  cout << "myID = " << ID << endl;
  cout << "+ nRels = " << r.getSize() << endl;
  cout << "+ nNbrs = " << getNNbrs()  << endl;
  vector<GLint> assignedIDs;
  for (GLint i = 0; i < r.getSize(); ++i)
  {
  closestDist     = GLfloat(RAND_MAX);
  closestNbrIndex = -1;
  Vector currRel;
  if (r.getHead(currRel))
  {
  for (GLint j = 0; j < getNNbrs(); ++j)
  {
  Neighbor currNbr;
  if (getHead(currNbr))
  {
  currDist = (getNbr(0)->relDesired - r[0]).magnitude();
  if (currDist < closestDist)
  {
  bool assignedID = false;
  for (GLint k = 0; k < assignedIDs.getSize(); ++k)
  {
  GLint currID = -1;
  if ((assignedIDs.getHead(currID)) &&
  (currID == getNbr(0)->ID))
  {
  assignedID = true;
  break;
  }
  ++assignedIDs;
  }
  if (!assignedID)
  {
  closestDist     = currDist;
  closestNbrIndex = j;
  }
  }
  }
  ++(*this);
  }
  if ((closestNbrIndex >= 0) && (closestNbrIndex < getNNbrs()))
  {
  assignedIDs.insertTail(getNbr(closestNbrIndex)->ID);
  cout << "  -nbrID[" << i << "] = "
  << getNbr(closestNbrIndex)->ID;
  cout << " | rel = " << r[0] << endl;
  getNbr(closestNbrIndex)->relDesired = r[0];
  ++r;
  }
  else
  {
  cout << "   -nbrID[" << i << "] = " << ID_NO_NBR << endl;
  r.removeHead();
  }
  }
  else ++r;
  }
  --ROSS--*/
  /*--ROSS--
    for (GLint i = 0; i < getNNbrs(); ++i)
    {
    closestDist     = GLfloat(RAND_MAX);
    closestRelIndex = -1;
    if (getHead(currNbr))
    {
    for (GLint j = 0; j < r.getSize(); ++j)
    {
    Vector currRel;
    if (r.getHead(currRel))
    {
    currDist = (getNbr(0)->relDesired - currRel).magnitude();
    if (currDist < closestDist)
    {
    closestDist     = currDist;
    closestRelIndex = j;
    }
    }
    ++r;
    }
    cout << "  -nbrID[" << i << "] = " << getNbr(0)->ID;
    if ((closestRelIndex >= 0) && (closestRelIndex < r.getSize()))
    {
    cout << " | rel = " << r[closestRelIndex] << endl;
    getNbr(0)->relDesired = r[closestRelIndex];
    r.remove(closestRelIndex);
    ++(*this);
    }
    else
    {
    cout << " | [DELETING]" << endl;
    removeHead();
    }
    }
    else ++(*this);
    }
  //setNbrs(nh);
  --ROSS--*/

  if (leftNbr  != NULL) leftNbr->relDesired  = r[LEFT_NBR_INDEX];
  if (rightNbr != NULL) rightNbr->relDesired = r[RIGHT_NBR_INDEX];
  return true;
}   // changeFormation(const Formation &, Neighbor)



//
// bool sendStateToNbrs()
// Last modified: 27Aug2006
//
// Attempts to broadcast the state of the cell
// to the neighborhood of the cell, returning
// true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:  <none>
//
bool Cell::sendStateToNbrs()
{
  Neighbor curr;
  if(VERBOSE)printf("cellID=%d\n",getID());
  for (GLint i = 0; i < getNNbrs(); ++i)
  {
    if(VERBOSE)printf("sending state to id= %d\n",getNbr(i)->ID);

    //if((getNbr(i)->ID)==NULL
    //if(VERBOSE)printf("

    if (!sendState(getNbr(i)->ID))
    {
      //printf("sendState returned false\n");
      return false;
    }
  }
  //printf("leaving sendStateToNbrs()\n");
  return true;
}   // sendStateToNbrs()



//
// bool sendState(toID)
// Last modified: 27Aug2006
//
// Attempts to send the state of the cell
// to the neighbor with the parameterized ID,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:
//      toID    in      the ID of the receiving neighbor
//
bool Cell::sendState(const GLint toID)
{
  //printf("in sendState()\n");
  State *s = new State(*this);
  //printf("number of relations of %d is %d \n",toID,s->rels.getSize());
  //printf("calling cell = %d\n", this->ID);
  //printf("number of relations of %d is %d \n",this->ID,this->rels.getSize());
  bool answer = sendMsg(s, toID, STATE);
  //printf("leaving sendState()\n");
  return answer;
}   // sendState(const GLint)



//
// bool processPackets()
// Last modified: 30Nov2009
//
// Attempts to process all packets received by the cell,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:  <none>
//
bool Cell::processPackets()
{
  bool   success = true;
  Packet p;
  while (!msgQueue.empty())
  {
    p = msgQueue.front();
    if (!processPacket(p)) success = false;
    msgQueue.pop();
  }
  return success;
  msgQueue.pop();
}   // processPackets()



//
// bool processPacket(p)
// Last modified: 22Dec2006
//
// Attempts to process the parameterized packet,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:
//      p       in/out      the packet to be processed
//
bool Cell::processPacket(Packet &p)
{
  bool success = false;
  if ((p.fromOperator()) && (p.type == CHANGE_FORMATION))
  {
    success  = changeFormation(*((Formation *)p.msg));
  }else if(p.type == AUCTION_ANNOUNCEMENT)
  {
    if(ALLOW_CELL_BIDS)
    {


    }
    success = true;
  }
  else if(p.type == BID)
  {
    if(p.msg!=NULL)
    {
      bids.push_back((Bid*)p.msg);
      success = true;
      numBids++;
      //cout << "bid received, total = " << numBids << endl;
    }
    //PROP_MESSAGE may need to be some kind of prop super-enum that covers the base for all enum_props
    //
  } else if (((isNbr(p.fromID)) || (p.fromBroadcast())) && !(p.type == NCELL_REQUEST ||p.type == NCELL_RESPONSE ||
        (p.type == FCNTR_RESPONSE || p.type == FCNTR_REQUEST) ||
        (p.type == FRAD_RESPONSE || p.type == FRAD_REQUEST) ||
        (p.type == FSEED_RESPONSE || p.type == FSEED_REQUEST)))
  {
    switch(p.type)
    {
      case STATE:
        success = (p.msg == NULL) ?
          false : updateNbr(p.fromID, *((State *)p.msg));
        delete (State *)p.msg;
        p.msg   = NULL;
        break;
      default: break;
    }
  }
  //receive a request, send out a request to nbrs that ref you
  else if(p.type == NCELL_REQUEST || p.type == NCELL_RESPONSE)
  {
    success = processNCell(p);
  }
  //receiving a response back from nbrs that ref you
  else if(p.type == FCNTR_REQUEST || p.type == FCNTR_RESPONSE)
  {
    success = processFcntr(p);
  }
  else if(p.type == FRAD_REQUEST || p.type == FRAD_RESPONSE)
  {
    success = processFRad(p);
  }
  else if (p.type == FSEED_REQUEST || p.type == FSEED_RESPONSE)
  {
    success = processFSeed(p);
  }

  return success;
}   // processPacket(Packet &)

bool Cell::processNCell(Packet &p)
{
  bool success = false;
  if(p.type == NCELL_REQUEST)
  {
    //If size is != 0, clear out the entire list and re-calculate.
    //wonder if the messages are still floating around
    //if information has been calculated previously, we need to erase it,
    //and recalculate it
    //this doesnt work for radius since the radius is computed only if NCELLS
    //and FCNTR have been calculated
    //
    if(props.size() != 0)
    {
      props.erase(props.begin(), props.end());
      success = true;
    }


    bool is_ref_nbr = false;
    Neighbor nbr;
    //loop through nbrs
    for(GLint i = 0; i < getNNbrs(); ++i)
    {
      nbr = at(i);

      if(nbr.refID == ID) //this nbr references you!
      {
        //send msg on to this nbr.
        //make copy of p.msg, push it onto props, fwd on the message to this nbr
        //
        PropMsg *propm = new PropMsg(nbr.ID, 0, false);//why do i have params
        props.push_back(*propm);                       //here and not elsewhere
        env->sendMsg(p.msg, nbr.ID,ID,NCELL_REQUEST); 
        is_ref_nbr =true;
        success=true;
      }
      if(i == size()-1 && is_ref_nbr == false) //no one has ref'd you.
      {
        //if this nbr doesnt ref you, and none have, you're at edge,
        //send response msg with count +1;
        // cout << "at and edge with ID: "
        // << ID << " and my gradient is: " << gradient << endl;
        PropMsg *prop = new PropMsg();
        (*(PropMsg *)prop).count = 1; 
        env->sendMsg(prop, refID,ID, NCELL_RESPONSE);
        success=true;
      }
    }

  }
  else if (p.type == NCELL_RESPONSE)
  {

    Neighbor nbr;
    bool all_response = false;
    // cycle through nbrs in your props list?
    for(GLint i = 0; i < getNNbrs(); ++i)
    {
      nbr = at(i);
      //check if msg came from a nbr who references you
      if(p.fromID == nbr.ID && nbr.refID == ID)
      {
        for(GLint j = 0; j < props.size(); ++j)
        {
          if(props.at(j).toID == p.fromID) //msg came from a cell we sent to
          {
            props.at(j).response = true;
            //following statement grabs the most recent "count" that's out there,
            props.at(j).count = (*((PropMsg *)p.msg)).count; //FCntrMsg.gradient
          }
        }
        for(GLint j = 0; j <props.size(); ++j)
        {
          if(props.at(j).response == false)
          {
            all_response = false;
            break;
          }
          all_response = true; //if you get here, you didnt break, all must be true
        }

        if(all_response)
        {
          int sum = 1;
          //cout << "gradient of " << ID << " is: " << gradient << endl;
          for(GLint k=0; k < props.size(); ++k)
          {
            sum += props.at(k).count;
          }
          if(refID == -1) //seed
          {
            cout << endl << endl;
            cout << endl << endl;
            cout << "Count of the robots is => " << sum << endl;
            success = true;
          }
          else
          {
            PropMsg *prop = new PropMsg();
            (*(PropMsg *)prop).count = sum; //FCntrMsg.gradient 
            env->sendMsg(prop, refID,ID, NCELL_RESPONSE);
            success = true;
          }
        }
      }
    }
  }
  return success;
} //processNCell()

bool Cell::processFcntr(Packet &p)
{
  //If size is != 0, clear out the entire list and re-calculate.
  bool success = false;
  if(p.type == FCNTR_REQUEST)
  {
    if(props.size() != 0)
    {
      props.erase(props.begin(), props.end());
      success = true;

    }
    bool is_ref_nbr = false;
    Neighbor nbr;
    //loop through nbrs
    for(GLint i = 0; i < getNNbrs(); ++i)
    {
      nbr = at(i);

      if(nbr.refID == ID) //this nbr references you!
      {
        //send msg on to this nbr.
        //make copy of p.msg, push it onto props, fwd on the message to this nbr
        //
        PropMsg *propm = new PropMsg(nbr.ID,0, 0, false);
        props.push_back(*propm); // a pointer? shouldnt it be a copy?
        env->sendMsg(p.msg, nbr.ID,ID,FCNTR_REQUEST); 
        is_ref_nbr =true;
        success=true;
      }
      if(i == size()-1 && is_ref_nbr == false) //no one has ref'd you.
      {
        //if this nbr doesnt ref you, and none have, you're at edge,
        //send response msg with count +1;
         cout << "at and edge with ID: "
         << ID << " and my gradient is: " << gradient << endl;
        PropMsg *prop = new PropMsg();
        (*(PropMsg *)prop).count = 1;
				Vector transErrorCopy = transError;
				transErrorCopy.rotateRelative(rotError + formation.getHeading());
        (*(PropMsg *)prop).gradient = gradient - transErrorCopy;
        env->sendMsg(prop, refID,ID, FCNTR_RESPONSE);
        success=true;
      }
    }
  }
  else if (p.type == FCNTR_RESPONSE)
  {

    Neighbor nbr;
    bool all_response = false;
    //maybe cycle through nbrs in your props list?
    for(GLint i = 0; i < getNNbrs(); ++i)
    {
      nbr = at(i);
      //if msg came from a nbr who references you
      if(p.fromID == nbr.ID && nbr.refID == ID)
      {
        for(GLint j = 0; j < props.size(); ++j)
        {
          if(props.at(j).toID == p.fromID) //msg came from a cell we sent to
          {
            props.at(j).response = true;
            //grab p.msg's count, set it to that vector position's count
            //for later summing up, only need to do this if props.at(j).count < 1.
            //if(props.at(j).count <= 1)
            //{
              /*cout << "current p.msg count: " << (*((PropMsg *)p.msg)).count   
                   << " and position " << (*((PropMsg *)p.msg)).gradient
                   << " for cell: " << ID << endl;*/
              props.at(j).count = (*((PropMsg *)p.msg)).count; 
              props.at(j).gradient = (*((PropMsg *)p.msg)).gradient;
           // }
          }
        }
        for(GLint j = 0; j <props.size(); ++j)
        {
          if(props.at(j).response == false)
          {
            all_response = false;
            break;
          }
          all_response = true; //if you get here, you didnt break, all must be true
        }

        if(all_response)
        {
          int sum = 1;
				  Vector transErrorCopy = transError;
				  transErrorCopy.rotateRelative(rotError + formation.getHeading());
          Vector gradSum = gradient - transErrorCopy;
          cout << "gradient of " << ID << " is: " << gradient << endl;
          for(GLint k=0; k < props.size(); ++k)
          {
            sum += props.at(k).count;
            gradSum += props.at(k).gradient;
          }
          if(refID == -1) //seed
          {
            cout << endl << endl;
            cout << endl << endl;
            cout << "sum of gradients is => " << gradSum << endl;
            cout << "Count of the robots is => " << sum << endl;
            //gradSum / sum for the center of mass? needs overloaded in vector
            Vector mid = gradSum*(1.0f/(float)sum);
            cout << "Middle of automaton is => " << mid  << endl;
            //cout <<"Magnitude is: " << mid.magnitude() << endl;
            //draw x at location
            env->getCentroid(mid); 
            success = true;
          }
          else
          {
            PropMsg *prop = new PropMsg();
            (*(PropMsg *)prop).count = sum; //FCntrMsg.gradient 
            (*(PropMsg *)prop).gradient = gradSum; //FCntrMsg.gradient 
            env->sendMsg(prop, refID,ID, FCNTR_RESPONSE);
            success = true;
          }
        }
      }
    }
  }
  return success;
}
bool Cell::processFRad(Packet &p)
{
  //If size is != 0, clear out the entire list and re-calculate.
  bool success = false;
  if(p.type == FRAD_REQUEST)
  {
    bool is_ref_nbr = false;
    Neighbor nbr;
    //loop through nbrs
    for(GLint i = 0; i < getNNbrs(); ++i)
    {
      nbr = at(i);

      if(nbr.refID == ID) //this nbr references you!
      {
        //send msg on to this nbr.
        //make copy of p.msg, push it onto props, fwd on the message to this nbr
        //
        PropMsg *propm = new PropMsg(nbr.ID,0, 0.0f, 0, false); //needed for FRAD?
        props.push_back(*propm); // a pointer? shouldnt it be a copy?
        env->sendMsg(p.msg, nbr.ID,ID,FRAD_REQUEST); 
        is_ref_nbr =true;
        success=true;
      }
      if(i == size()-1 && is_ref_nbr == false) //no one has ref'd you.
      {
        //if this nbr doesnt ref you, and none have, you're at edge,
        //send response msg with count +1;
        PropMsg *prop = new PropMsg();
				Vector transErrorCopy = transError;
				transErrorCopy.rotateRelative(rotError + formation.getHeading());
        (*(PropMsg *)prop).radius = ((gradient-transErrorCopy)-env->centroid).magnitude(); //calculation;
        env->sendMsg(prop, refID, ID, FRAD_RESPONSE);
        success=true;
      }
    }
  }
  else if (p.type == FRAD_RESPONSE)
  {

    Neighbor nbr;
    bool all_response = false;
    // cycle through nbrs in your props list?
    for(GLint i = 0; i < getNNbrs(); ++i)
    {
      nbr = at(i);
      //if msg came from a nbr who references you
      if(p.fromID == nbr.ID && nbr.refID == ID)
      {
        for(GLint j = 0; j < props.size(); ++j)
        {
          if(props.at(j).toID == p.fromID) //msg came from a cell we sent to
          {
            props.at(j).response = true;
              props.at(j).radius = (*((PropMsg *)p.msg)).radius;
          }
        }
        for(GLint j = 0; j <props.size(); ++j)
        {
          if(props.at(j).response == false)
          {
            all_response = false;
            break;
          }
          all_response = true; //if you get here, you didnt break, all must be true
        }

        if(all_response)
        {
          //cout << "gradient of " << ID << " is: " << gradient << endl;
				  Vector transErrorCopy = transError;
				  transErrorCopy.rotateRelative(rotError + formation.getHeading());
          GLfloat maxRadius =  ((gradient-transErrorCopy)-env->centroid).magnitude();
          for(GLint k=0; k < props.size(); ++k)
          {
            //compute max radius between mine and the best in my list
            if(maxRadius < props.at(k).radius)
            {
              maxRadius = props.at(k).radius; 
            }
          }
          if(refID == -1) //seed
          {
            cout << "Radius = " << maxRadius << endl;
            env->getRadius(maxRadius);
            success = true;
          }
          else
          {
            PropMsg *prop = new PropMsg();
            (*(PropMsg *)prop).radius = maxRadius; //FCntrMsg.gradient 
            env->sendMsg(prop, refID,ID, FRAD_RESPONSE);
            success = true;
          }
        }
      }
    }
  }
  return success;
}
bool Cell::processFSeed(Packet &p)
{
  //If size is != 0, clear out the entire list and re-calculate.
  bool success = false;
  if(p.type == FSEED_REQUEST)
  {
    bool is_ref_nbr = false;
    Neighbor nbr;
    //loop through nbrs
    for(GLint i = 0; i < getNNbrs(); ++i)
    {
      nbr = at(i);

      if(nbr.refID == ID) //this nbr references you!
      {
        //send msg on to this nbr.
        //make copy of p.msg, push it onto props, fwd on the message to this nbr
        //
        PropMsg *propm = new PropMsg(nbr.ID,0, 0, 0.0f, 0, false); 
        props.push_back(*propm); // a pointer? shouldnt it be a copy?
        env->sendMsg(p.msg, nbr.ID,ID,FSEED_REQUEST); 
        is_ref_nbr =true;
        success=true;
      }
      if(i == size()-1 && is_ref_nbr == false) //no one has ref'd you.
      {
        //if this nbr doesnt ref you, and none have, you're at edge,
        //send response msg with count +1;
        PropMsg *prop = new PropMsg();
        //may hve to do the same trick with error as i did with radius
				//Vector transErrorCopy = transError;
				//transErrorCopy.rotateRelative(rotError + formation.getHeading());
        (*(PropMsg *)prop).distance = (gradient- env->centroid);
        env->sendMsg(prop, refID, ID, FSEED_RESPONSE);
        success=true;
      }
    }
  }
  else if (p.type == FSEED_RESPONSE)
  {

    Neighbor nbr;
    bool all_response = false;
    //maybe cycle through nbrs in your props list?
    for(GLint i = 0; i < getNNbrs(); ++i)
    {
      nbr = at(i);
      //if msg came from a nbr who references you
      if(p.fromID == nbr.ID && nbr.refID == ID)
      {
        for(GLint j = 0; j < props.size(); ++j)
        {
          if(props.at(j).toID == p.fromID) //msg came from a cell we sent to
          {
            props.at(j).response = true;
              props.at(j).distance = (*((PropMsg *)p.msg)).distance;
          }
        }
        for(GLint j = 0; j <props.size(); ++j)
        {
          if(props.at(j).response == false)
          {
            all_response = false;
            break;
          }
          all_response = true; //if you get here, you didnt break, all must be true
        }

        if(all_response)
        {
          //cout << "gradient of " << ID << " is: " << gradient << endl;
				  //Vector transErrorCopy = transError;
				  //transErrorCopy.rotateRelative(rotError + formation.getHeading());
         // GLfloat maxRadius =  ((gradient-transErrorCopy)-env->centroid).magnitude();
          GLfloat minDist = (gradient - env->centroid).magnitude();
          Vector minVector = (gradient - env->centroid);
          for(GLint k=0; k < props.size(); ++k)
          {
            //compute max radius between mine and the best in my list
            if(minDist > (props.at(k).distance).magnitude())
            {
              minDist = (props.at(k).distance).magnitude(); 
              minVector = props.at(k).distance;
            }
          }
          if(refID == -1) //seed
          {
            cout << "Distance = " << minDist << endl;
            cout << "Cell at " << (env->centroid)+minVector 
                 << " should be seed." << endl;
            env->getDistance(env->centroid+minVector);
            success = true;
          }
          else
          {
            PropMsg *prop = new PropMsg();
            (*(PropMsg *)prop).distance = minVector ; //FCntrMsg.gradient 
            env->sendMsg(prop, refID,ID, FSEED_RESPONSE);
            success = true;
          }
        }
      }
    }
  }
  return success;
}

// <public primitive behaviors>

//
// Behavior moveError()
// Last modified: 07Nov2009
//
// Moves the robot cell using the current translational and
// rotational errors, activating and returning the appropriate
// robot behavior.
//
// Returns:     the appropriate robot behavior
// Parameters:  <none>
//
Behavior Cell::moveError()
{
  return behavior = moveErrorBehavior(transError, rotError);
}   // moveError()



//
// Behavior moveError(tError, rError)
// Last modified: 07Nov2009
//
// Moves the robot cell using the parameterized translational and
// rotational errors, activating and returning the appropriate
// robot behavior.
//
// Returns:     the appropriate robot behavior
// Parameters:
//      tError  in      the translational error
//      rError  in      the rotational error
//
Behavior Cell::moveError(const Vector tError, const GLfloat rError)
{
  return behavior = moveErrorBehavior(tError, rError);
}   // moveError(const Vector, const GLfloat)



//
// Behavior moveErrorBehavior(tError, rError)
// Last modified: 07Nov2009
//
// Moves the robot using the parameterized translational and
// rotational errors, returning the appropriate robot behavior.
//
// Returns:     the appropriate robot behavior
// Parameters:
//      tError  in      the translational error
//      rError  in      the rotational error
//
Behavior Cell::moveErrorBehavior(const Vector tError, const GLfloat rError)
{
  if      (transError.magnitude() > threshold())
    return moveArc(transError);
  else if (abs(rotError)          > angThreshold())
    return moveArc(0.0, degreesToRadians(-rotError));
  return moveStop();
}   // moveErrorBehavior(const Vector, const GLfloat)



// <virtual overloaded operators>

//
// Cell& =(s)
// Last modified: 27Aug2006
//
// Copies the contents of the parameterized state into this cell.
//
// Returns:     this cell
// Parameters:
//      s       in/out      the state being copied
//
Cell& Cell::operator =(const State &s)
{
  return *this = s;
}   // =(const State &)



//
// Cell& =(nh)
// Last modified: 27Aug2006
//
// Copies the contents of the parameterized neighborhood into this cell.
//
// Returns:     this cell
// Parameters:
//      nh      in/out      the neighborhood being copied
//
Cell& Cell::operator =(const Neighborhood &nh)
{
  return *this = nh;
}   // =(const Neighborhood &)



//
// Cell& =(r)
// Last modified: 27Aug2006
//
// Copies the contents of the parameterized robot into this cell.
//
// Returns:     this cell
// Parameters:
//      r       in/out      the robot being copied
//
Cell& Cell::operator =(const Robot &r)
{
  return *this = r;
}   // =(const Robot &)



// <virtual protected utility functions>

//
// bool init(dx, dy, dz, theta, colorIndex)
// Last modified: 30Nov2009
//
// Initializes the cell to the parameterized values,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:
//      dx          in      the initial x-coordinate of the cell (default 0)
//      dy          in      the initial y-coordinate of the cell (default 0)
//      dz          in      the initial z-coordinate of the cell (default 0)
//      theta       in      the initial heading of the cell (default 0)
//      colorIndex  in      the initial array index of the color of the cell
//
bool Cell::init(const GLfloat dx,    const GLfloat dy, const GLfloat dz,
    const GLfloat theta, const Color   colorIndex)
{
  showFilled = DEFAULT_CELL_SHOW_FILLED;
  leftNbr    = rightNbr = NULL;
  auctionStepCount = 0;
  return true;
}   // init(const GLfloat..<4>, const Color)


void Cell::settleAuction()
{
  //cout << "Cell::settleAuction() entered\n"<< endl;
  auctionStepCount = 0;
  if(bids.size()<=0)
  {
    return;
  }
  Bid* winningBid;
  winningBid = bids[0];
  for(int i=0;i<bids.size();i++)
  {
    if(bids[i]->b_i < winningBid->b_i)
    {
      winningBid = bids[i];
    }
  }
  //cout <<"Robot # "<<winningBid->rID<<" won the auction" << endl;
  env->settleAuction(this,winningBid->rID);
  bids.clear();
}

GLint Cell::getNBids() const
{
  return bids.size();
}

int Cell::getAuctionStepCount() const
{
  return auctionStepCount;
}

bool Cell::setAuctionStepCount(const int& asc)
{
  auctionStepCount = asc;
  return true;
}
