//
// Filename:        "PropMsg.h"
//
// Programmer:      Brent Beer
// Last modified:   25Feb2010
//
// Description:     This structure describes a property message that
//                  is used as a state variable to keep track if a property
//                  message that was requested has had a response.
//

#ifndef PROPMSG_H
#define PROPMSG_H
#include <vector>
#include "Vector.h"
#include "GLIncludes.h"

using namespace std;

struct PropMsg
{

  // <data members>
  GLint toID;    //ID of the robot the message was sent to.
  Vector gradient;  //count of the number of responses summed up so far.
  Vector distance; //distance of the cell who is currently closest to the FCNTR
  GLfloat radius;
  GLint count;
  bool response; //boolean of wheather or not the msg has been received.
  //msgID as a (long)timestampto ensure that the message you send out for
  //a request is the same you're getting back on a response

  // <constructor>
  
  //
  // PropMsg(to, from, answer)
  // Last modified: 25Feb2010
  //
  // This is the default constructor that initializes
  // this class to its parameterized values
  //
  // Returns: <none>
  //
  // Parameters: 
  //     toID      in         the default ID of recipient 
  //     count     in         the default count of responses.
  //     response  in         the default of state of a response
  PropMsg(const GLint     to         = -1,
          const Vector    grad       = Vector(),
          const Vector    dist       = Vector(),
          const GLfloat   rad        = 0.0f,
          const GLint     num        = 0,
          const bool      answer     = false)
          : toID(to), gradient(grad),distance(dist) ,radius(rad),
          count(num), response(answer)
  {
  }//PropMsg(const..{GLint, GLint,bool})
}; //PropMsg
#endif
