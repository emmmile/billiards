


//#define DEBUG		1

#include <wx/control.h>
#include <wx/gdicmn.h>
#include <wx/timer.h>
#include <wx/event.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <vector>
#include <list>
#include <sstream>
#include <string>
#include <iostream>
#include "BilliardObject.h"
using namespace std;



//#define NATIVEZOOMING	0

#define frand1()	( ( random() << 1 ) * 4.65661287524579692411E-10 )




#define W		800

#define WHALF		( W * 0.5 )

#define H		( W * 0.5 )

#define HHALF		( H * 0.5 )

#define MINZOOM		1.0

#define SCROLLSTEP	1.0

#define ZOOMSTEP	1.1

#define MAXCUEDISTANCE	50

// intervallo di aggiornamento del controllo (msec)
#define DT		20

// dimensione delle palline
#define RADIUS		( 0.015 * W ) //12.0

// dimensione bottoni
#define BR		10.0

#define BRI		3.5

#define INNER		( RADIUS * 0.72 )

// attrito (dipende anche da DT)
#define FRICTION  	0.985	// abbastanza ok con vel max = 2.0

#define BORDER		RADIUS

// bordo sull'angolo
#define ABORDER		( 3.2 * BORDER )

// bordo orizzontale nel mezzo 
#define OBORDER		( ABORDER * M_SQRT2 * 0.5 )

// raggio delle buche
#define HOLE		( ABORDER * 0.8 )

// quando la velocita' e' piu' piccola di questo valore la setto a 0
#define EPSILON		1.0E-3

#define DEPSILON	1.0E-6

#define VEPSILON	1.0E-6

#define TEPSILON	1.0E-8

#define REALRADIUS	( RADIUS + DEPSILON )

#define CUEHEADL	16.0

#define CUEHEADH	3.0

#define CUELEN		( H * 1.25 )

// moltiplicazione per costante, che mancava
inline wxRealPoint operator* ( double k, const wxRealPoint &v ) {
	return wxRealPoint( v.x * k, v.y * k );
}

// modulo al quadrato del vettore v
inline double mod2 ( const wxRealPoint &v ) {
	return v.x * v.x + v.y * v.y;
}

inline wxRealPoint normalize ( const wxRealPoint& v ) {
	double a = 1.0 / sqrt( mod2( v ) );
	return wxRealPoint( v.x * a, v.y * a );
}

// restituisce un vettore perpendicolare a v
inline wxRealPoint perpendicular ( const wxRealPoint& v ) {
	return wxRealPoint( v.y, -v.x );
}

// restituisce un vettore (in coordinate canoniche) con le coordinate cambiate rispetto alla nuova base (b1, b2)
inline wxRealPoint changeCoordinates ( const wxRealPoint& v, const wxRealPoint& b1, const wxRealPoint& b2 ) {
	// in pratica moltiplico v per la matrice le cui colonne sono b1 e b2
	return wxRealPoint( v.x * b1.x + v.y * b1.y, v.x * b2.x + v.y * b2.y );	
}


#if DEBUG
inline string str ( const wxRealPoint& v ) {
	ostringstream s;
	s << "(" << v.x << ", " << v.y << ")";
	return s.str();
}
#endif


enum buttonState { NORMAL, TOUCHED, PRESSED };
enum scrollType { UP, DOWN, LEFT, RIGHT, NONE };

class Billiard;

// per ora definisco solo questo tipo di bottoni
class ButtonArrow {
public:
	scrollType id;
	buttonState status;
	wxRealPoint pos;
	wxPoint arrow[3];
	
	ButtonArrow ( scrollType id, wxRealPoint pos );
	
	void draw ( wxBufferedDC& dc );
	bool isInside ( const wxRealPoint& p );
	void press ( Billiard& b );
	void touch ( Billiard& b );
	void leave ( Billiard& b );
};





// TODO aggiungere gli altri stati
enum gameStatus { PLAYING, POSITION };


class Billiard : wxControl {
public:	

	wxColor	backColor;
	
	
	
	wxTimer timer;
	list<Ball> balls;
	
	
	vector<Side> sides;
	vector<Hole> holes;
	//double friction;
	bool stopped;
	wxWindow* w;
	wxRealPoint mouse, mouseAbsolute;
	static double zoom;
	wxRealPoint offset;
	vector<ButtonArrow> buttons;
	scrollType scrollStatus;
	bool onButton;
	gameStatus game;
	ButtonArrow* active;
	double cueDistance;
	
	wxRealPoint cue [6];
	wxPoint cuee[6];
	double speed;
	wxRealPoint cueDir;
	
	
	Billiard ( wxWindow* w );
	~Billiard ( );
	
	
	void init ( );
	void clear ( );
	void scroll ( );
	void changeZoom ( bool, bool );
	void calcPhysix ( );
	bool everythingStopped ( );
	void positionBalls ( int, uint, uint, uint, uint );
	bool ballPositionIsOk ( Ball& b );
	
	void Command ( wxCommandEvent& e );
	void onMouseClick ( wxMouseEvent& e );
	void onMouseUp ( wxMouseEvent& e );
	void onMouseMove ( wxMouseEvent& e );
	void onMouseWheel ( wxMouseEvent& e );
	void onTimer ( wxTimerEvent& e );
	void onSize ( wxSizeEvent& e );
	void onKeyUp ( wxKeyEvent& e );
	void onPaint ( wxPaintEvent& );
	void onKeyDown ( wxKeyEvent& e );
	
	void drawTable ( wxBufferedDC& dc );
	void drawBalls ( wxBufferedDC& dc );
	void drawCue ( wxBufferedDC& dc );
	void drawButtons ( wxBufferedDC& dc );
	
	
	//#if DEBUG
	void randomBalls( int, uint, uint, uint, uint );
	void randomBall( int, uint, uint, uint, uint );
	void randomSpeeds( double n );
	//#endif
	
	DECLARE_EVENT_TABLE()
};







