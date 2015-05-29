
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <cmath>

#define ORIZONTAL 	false
#define	VERTICAL	true

#define EASY		false
#define HARD		true

#define FREE		EASY
#define STATIC		HARD

class Ball;

// classe che denota un oggetto fisico. Puo' essere colpito e disegnato
class BilliardObject {
public:
	virtual bool collide ( Ball& b, double& dt ) = 0;
	virtual void draw ( wxBufferedDC& dc ) = 0;
};



class Ball : public BilliardObject {
public:
	// la classe wxRealPoint ha gia' gli operatori vettoriali! (+, -, ...)
	wxRealPoint pos, vel;
	int num;
	wxString n;
	wxColor color;
	
	Ball( ) { }
	Ball( wxRealPoint pos, int num );
	
	void updateAfter ( double dt );
	bool collide ( Ball& b );
	bool collide ( Ball& b, double& dt );
	bool isStopped ( );
	void verySlow ( );
	void draw ( wxBufferedDC& dc );
	
	void randomSpeed( double max );	
	
	#if DEBUG
	void print ( );
	bool isOut ( );
	#endif
};



class Side : public BilliardObject {
public:
	wxRealPoint beg, end;
	Ball a, b;
	Ball tmp;
	int type;
	double dir;
	
	Side( wxRealPoint beg, wxRealPoint end, int tipe, double dir );
	bool collide ( Ball& b, double& dt );
	void draw ( wxBufferedDC& dc );
	bool isInside ( wxRealPoint& v );
	bool collide ( Ball& b );
	#if DEBUG
	void print ( );
	#endif
};





class Hole : public BilliardObject {
public:
	wxRealPoint pos;
	
	//Hole( )
	Hole( wxRealPoint pos ) { this->pos = pos; }
	
	bool collide ( Ball& b );
	// viene usata
	bool collide ( Ball& b, double& dt ) { return collide( b ); }
	void draw ( wxBufferedDC& dc );
};


















// per gestire una collisione fra due oggetti generici.
class Collision {
public:
	BilliardObject* o, *p;
	double dt;
	
	virtual bool isCollision ( ) = 0;
	virtual void processCollision ( ) = 0;
};



class BallCollision : public Collision {
public:
	Ball aa, bb;
	wxRealPoint dir;
	wxRealPoint pdir;
	
	BallCollision ( ) { BallCollision( NULL, NULL, 0.0 ); }
	BallCollision ( Ball* a, Ball* b, double dt ) : aa(), bb() { 
		set( a, b, dt );
	}
	void set( Ball* a, Ball* b, double dt ) {
		this->o = (BilliardObject*) a;
		this->p = (BilliardObject*) b;
		this->dt = dt;	
	}
	bool isCollision ( );
	void processCollision ( );
	void processCollision( bool type );
	
	/*inline class BallCollision operator= ( const BallCollision& c ) {
		//this->s = c.s;
		this->o = c.o;
		this->p = c.p;
		this->aa = c.aa;
		this->bb = c.bb;
		this->dir = c.dir;
		this->pdir = c.pdir;
		this->dt = c.dt;
		return *this;
	}*/
};


class SideCollision : public Collision {
public:
	Ball bb;
	BallCollision c;
	bool type;
	
	SideCollision ( ) { SideCollision( NULL, NULL, 0.0 ); }
	SideCollision ( Side* a, Ball* b, double dt ) : bb() { 
		set( a, b, dt );
	}
	
	void set ( Side* a, Ball* b, double dt ) {
		this->o = (BilliardObject*) a;
		this->p = (BilliardObject*) b;
		this->dt = dt;
	}
	
	bool isCollision ( );
	void processCollision ( );
};


/*
class HoleCollision : public Collision {
public:
	HoleCollision ( ) { HoleCollision( NULL, NULL, 0.0 ); }
	HoleCollision ( Hole* a, Ball* b, double dt ) { 
		set( a, b, dt );
	}
	
	void set ( Hole* a, Ball* b, double dt ) {
		this->o = (BilliardObject*) a;
		this->p = (BilliardObject*) b;
		this->dt = dt;
	}
	
	bool isCollision ( );
	void processCollision ( );
};
*/


