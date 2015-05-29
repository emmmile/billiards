




#include <iostream>
#include <string>
#include <sstream>
#include "Billiard.h"
using namespace std;


//bool Hole::collide ( Ball& b ) { double d = DT; return collide( b, d ); }


Ball::Ball ( wxRealPoint pos, int num ) {
	this->pos = pos;
	this->num = num;
	
	// salvo il numero nell'etichetta
	ostringstream tmp;
	tmp << num;
	n = wxString( tmp.str().c_str(), wxConvUTF8 );
	
	if ( num ) color = *wxBLACK;
	else 	color = *wxWHITE;
}


void Ball::draw ( wxBufferedDC& dc ) {
	wxSize size ( dc.GetTextExtent( n ) );
	
	#if NATIVEZOOMING
	double z = 1.0;
	#else
	double z = Billiard::zoom;
	#endif
	
	dc.SetBrush( wxBrush( color ) );
	//cout << "Disegno la pallina numero " << balls[i].num << ".\n";
	
	dc.DrawCircle ( pos.x * z, pos.y * z, RADIUS * z );
	
	if ( num ) {
		dc.SetBrush( wxBrush( *wxWHITE ) );
		dc.DrawCircle ( pos.x * z, pos.y * z, INNER * z );
		dc.DrawText ( n, pos.x * z - 0.5 * size.GetWidth(), pos.y * z - 0.5 * size.GetHeight() );
	}
}


bool Ball::isStopped ( ) {

	return fabs( vel.x ) < EPSILON && fabs( vel.y ) < EPSILON;
}


bool Ball::collide ( Ball& b ) {
	return mod2( b.pos - pos ) < 4.0 * RADIUS * RADIUS;
}



// se le due palline si scontrano restituisce true e salva in dt il tempo prima
// dello scontro, altrimenti restituisce false.
bool Ball::collide ( Ball& b, double& dt ) {
	wxRealPoint x = b.pos - pos;
	wxRealPoint v = b.vel - vel;
	bool ret = false;
	//dt = DT;
	
	// questi sono calcoli che provengono dalla risoluzione di una equazione del tipo
	// sqrt( pos1 + dt*vel1 - pos2 + dt*vel2 ) == 2 * RADIUS
	double a = -4.0 * RADIUS * RADIUS + mod2( x );
	double c = -2.0 * ( v.x * x.x + v.y * x.y );
	double d = sqrt( c * c - 4.0 * a * mod2( v ) );
	a = 0.5 / mod2( v );
	
	double dt1 = a * ( c + d );
	double dt2 = a * ( c - d );
	
	
	//if ( dt1 >= 0.0 && fabs( dt1 ) < dt ) {
	if ( ( dt1 >= 0.0 && dt1 < dt ) || ( dt1 < 0.0 && fabs( dt1 ) < TEPSILON ) ) {
		//dt = dt1;
		dt = fabs( dt1 );
		ret = true;
	}
	
	//if ( dt2 >= 0.0 && fabs( dt2 ) < dt ) { 
	if ( ( dt2 >= 0.0 && dt2 < dt ) || ( dt2 < 0.0 && fabs( dt2 ) < TEPSILON ) ) {
		//dt = dt2;
		dt = fabs( dt2 );
		ret = true;
	}
	
	return ret;
}



// se le velocita' sono molto piccole allora le azzero del tutto
inline void Ball::verySlow ( ) {
	if ( fabs( vel.x ) < EPSILON ) 
		vel.x = 0.0;
		
	if ( fabs( vel.y ) < EPSILON ) 
		vel.y = 0.0;
}



void Ball::updateAfter ( double dt ) {
	verySlow();
		
	// aggiorno le variabili fisiche
	pos = pos + dt * vel;
	vel = FRICTION * vel;
}


void Ball::randomSpeed ( double max ) {
	vel.x = max * frand1();
	vel.y = max * frand1();
}


#if DEBUG
bool Ball::isOut ( ) {
	return pos.x < 0.0 || pos.x > W || pos.y < 0.0 || pos.y > H;
}


void Ball::print ( ) {
	cout << "pos = " << str( pos ) << ", vel = " << str( vel ) << endl;
}
#endif
















Side::Side ( wxRealPoint beg, wxRealPoint end, int type, double dir ) {
	this->beg = beg;
	this->end = end;
	this->type = type;
	this->dir = dir;
	
	if ( type == VERTICAL ) {
		a = Ball( wxRealPoint( beg.x + dir * RADIUS, beg.y ), -1 );
		b = Ball( wxRealPoint( end.x + dir * RADIUS, end.y ), -1 );
	} else {
		a = Ball( wxRealPoint( beg.x, beg.y + dir * RADIUS ), -1 );
		b = Ball( wxRealPoint( end.x, end.y + dir * RADIUS ), -1 );
	}
}



void Side::draw ( wxBufferedDC& dc ) {
	#if NATIVEZOOMING
	double z = 1.0;
	#else
	double z = Billiard::zoom;
	#endif
	
	dc.SetBrush( wxBrush( *wxWHITE ) );
	
	dc.DrawRoundedRectangle( ( beg.x - ( !type ? RADIUS : 0.0 ) ) * z, 
				 ( beg.y - ( type ? RADIUS : 0.0 ) ) * z, 
				 ( !type ? end.x - beg.x + 2.0 * RADIUS : dir * 100.0 ) * z,
				 ( type ? end.y - beg.y + 2.0 * RADIUS : dir * 100.0 ) * z, RADIUS * z );
	
}


inline bool Side::isInside ( wxRealPoint& v ) {
	if ( type == VERTICAL ) {
		return ( v.y >= beg.y ) && ( v.y <= end.y );
	} else 	return ( v.x >= beg.x ) && ( v.x <= end.x );;
}


// mi dice se c'e' o meno una collisione statica fra una pallina e il bordo
bool Side::collide ( Ball& ball ) {
	if ( a.collide( ball ) ) return true;
	if ( b.collide( ball ) ) return true;
	
	if ( type == VERTICAL ) {
		return isInside( ball.pos ) && fabs( beg.x - ball.pos.x ) < RADIUS;		
	} else 	return isInside( ball.pos ) && fabs( beg.y - ball.pos.y ) < RADIUS;
}

// questa invece considera anche la velocita' della pallina
// se viene trovata una collisione prima di DT salva in dt l'istante di tale collisione
// altrimenti restituisce false
bool Side::collide ( Ball& ball, double& dt ) {
	//wxRealPoint tmp;
	bool inside = false;
	double dt1 = dt;	// tempo di scontro con la pallina a (bordo su beg)
	double dt2 = dt;	// tempo di scontro con la pallina b (bordo su end)
	double dt3 = dt;	// tempo di scontro con la retta beg-end
	//double tmp = 0.0;
	//dt = DT;
	
	bool begCollision = a.collide( ball, dt1 );
	bool endCollision = b.collide( ball, dt2 );
	//bool ret = false;
	
	
	// guardo gli spigoli
	if ( begCollision ) {
		wxRealPoint v = ball.pos + dt1 * ball.vel;
		inside = isInside( v );
		if ( !inside ) {
			// al momento dell'urto con la pallina sono fuori dalla retta, quindi significa
			// che l'urto che ho trovato e' sicuramente il migliore
			dt = dt1;
			return true;
		}
	}
	
	if ( endCollision ) {
		wxRealPoint v = ball.pos + dt2 * ball.vel;
		inside = isInside( v );
		if ( !inside ) {
			dt = dt2;
			return true;
		}	
	}
	
	
	// guardo il lato dritto
	if ( type == VERTICAL && ( dir * ball.vel.x > 0.0 ) ) {
		dt3 = ( beg.x - ball.pos.x );
		dt3 += ( dt3 > 0.0 ) ? -RADIUS : +RADIUS;
		dt3 /= ball.vel.x;		
	} 
	
	if ( type == ORIZONTAL && ( dir * ball.vel.y > 0.0 ) ) {
		dt3 = ( beg.y - ball.pos.y );
		dt3 += ( dt3 > 0.0 ) ? -RADIUS : +RADIUS;
		dt3 /= ball.vel.y;
	}
	
	
	
	
	
	wxRealPoint v = ball.pos + dt3 * ball.vel;
	inside = isInside( v );
	
	if ( inside && dt3 >= 0.0 && dt3 < dt ) {
		dt = dt3;
		return true;
	}
	
	#if DEBUG > 1
	if ( begCollision ) { cout << "Collisione con l'estremo BEG in " << dt1 << endl; cout << inside << endl; }
	if ( endCollision ) { cout << "Collisione con l'estremo END in " << dt2 << endl; cout << inside << endl; }
	#endif
	
	
	return false;
}


#if DEBUG
void Side::print ( ) {
	cout << "beg = " << str( beg ) << ", end = " << str( end ) << endl;
}
#endif























// anche qui testo la collisione statica, restituisco true se il centro della pallina oltrepassa
// il bordo della buca
bool Hole::collide ( Ball& b ) {
	//return ( mod2( b.pos - pos ) < ( HOLE - RADIUS ) * ( HOLE - RADIUS ) );
	return ( mod2( b.pos - pos ) < ( HOLE ) * ( HOLE ) );
}



void Hole::draw ( wxBufferedDC& dc ) {
	dc.SetBrush( wxBrush( *wxBLACK ) );
	#if NATIVEZOOMING
	double z = 1.0;
	#else
	double z = Billiard::zoom;
	#endif
	
	dc.DrawCircle( pos.x * z, pos.y * z, HOLE * z );
}
















bool BallCollision::isCollision ( ) {
	Ball* a = (Ball*) o;
	Ball* b = (Ball*) p; 
	if ( a->isStopped() && b->isStopped() ) return false;
	
	
	aa.pos = a->pos + dt * a->vel;
	bb.pos = b->pos + dt * b->vel;
				
	//cout << "Distance now is: " << sqrt( mod2( c.a->pos - c.b->pos ) ) << endl;
	dir = normalize( aa.pos - bb.pos );	// direzione dell'urto
	pdir = perpendicular( dir );	// perpendicolare alla direzione
	
	
	
	
	// cambio le velocita' nelle nuove coordinate (la base e' data dalla direzione)
	aa.vel = changeCoordinates( a->vel, dir, pdir );
	bb.vel = changeCoordinates( b->vel, dir, pdir );
	
	#if DEBUG > 1
	/*//cout << "Posizione " << a-> num << " (A): " << str( aa.pos ) << endl;
	//cout << "Posizione " << b-> num << " (B): " << str( bb.pos ) << endl;	
	cout << "Direzione " << str( dir ) << endl;
	
	//cout << "Perpendicolare " << str( pdir ) << endl;
	cout << "Vel A (" << a->num << "): " << str( a->vel ) << " Pos A: " << str(a->pos) << endl;
	cout << "Vel B (" << b->num << "): " << str( b->vel ) << " Pos B: " << str(b->pos) << endl;
	
	cout << "Vel A sulla direzione: " << aa.vel.x << endl;
	cout << "Vel B sulla direzione: " << bb.vel.x << endl;*/
	#endif
		
	// casi che vanno bene:
	// (1) velocita' di segno opposto
	bool opposite = bb.vel.x >= 0.0 && aa.vel.x <= 0.0;
	// (2) velocita' positive, quella che "spinge" ha velocita' maggiore
	bool negative = ( aa.vel.x >= 0.0 || fabs( aa.vel.x ) < VEPSILON ) && 
			( bb.vel.x >= 0.0 || fabs( bb.vel.x ) < VEPSILON ) && 
			  fabs( bb.vel.x ) >= fabs( aa.vel.x );
	// (3) velocita' negative, ancora quella che "spinge" ha velocita', in modulo, maggiore
	bool positive = ( aa.vel.x <= 0.0 || fabs( aa.vel.x ) < VEPSILON ) &&
			( bb.vel.x <= 0.0 || fabs( bb.vel.x ) < VEPSILON ) &&
			  fabs( aa.vel.x ) >= fabs( bb.vel.x );
	//bool small = fabs( bb.vel.x ) < VEPSILON && fabs( aa.vel.x ) < VEPSILON;
	
	//cout << "E' un urto? " << opposite << negative << positive << endl;
	
	if ( !( opposite || negative || positive ) ) {
		#if DEBUG
		cout << "Non e' un urto.\n";
		#endif
		return false;
	}
	
	
	return true;
}




// Esegue i calcoli necessari quando viene trovato un urto fra palline
// se type e' true la collisione avviene con una pallina inamovibile (quelle posizionate sui bordi)
// altrimenti con una pallina normale
void BallCollision::processCollision ( bool type ) {
	Ball* a = (Ball*) o;
	Ball* b = (Ball*) p;
	
	if ( type == FREE ) {
		//if ( fabs( aa.vel.x ) < DEPSILON ) aa.vel.x = 0.0;
		//if ( fabs( bb.vel.x ) < DEPSILON ) bb.vel.x = 0.0;
		// scambio le velocita' sulla direzione dell'urto
		double tmp = aa.vel.x;
		aa.vel.x = bb.vel.x;
		bb.vel.x = tmp;
	} else  bb.vel.x = -bb.vel.x;
	
	// calcolo le velocita' finali (ricambio coordinate)
	// TODO anche se la changeCoordinates sono 2 somme e 4 moltiplicazioni
	// qui penso si potrebbe ottimizzare un po'..
	wxRealPoint b3 = changeCoordinates( wxRealPoint( 1.0, 0.0 ), dir, pdir );
	wxRealPoint b4 = changeCoordinates( wxRealPoint( 0.0, 1.0 ), dir, pdir );
	aa.vel = changeCoordinates( aa.vel, b3, b4 );
	bb.vel = changeCoordinates( bb.vel, b3, b4 );
	
	
	// setto le nuove velocita' nelle palline puntate
	if ( type == FREE ) {
		a->vel = aa.vel;
		a->pos = aa.pos;
		a->verySlow();
	}
	b->vel = bb.vel;
	b->pos = bb.pos;
}





// devo aver gia' chiamato una isCollision() sullo stesso oggetto
void BallCollision::processCollision ( ) {	
	processCollision( FREE );	
}




bool SideCollision::isCollision ( ) {
	Side* s = (Side*) o;
	Ball* b = (Ball*) p;	// pallina vera
	Ball* d;		// pallina fittizzia per gestire il bordo stondato
	bool ret;
	
	#if DEBUG > 1
	cout << "Guardo se c'e' veramente scontro fra la pallina ";
	b->print();
	cout << "e il bordo ";
	s->print();
	#endif
	
	// posizione nuova della pallina
	bb.pos = b->pos + dt * b->vel;
	bb.vel = b->vel;
	
	
	// gestione scontro col bordo
	if ( s->isInside( bb.pos ) ) {
		#if DEBUG > 1
		cout << "Controllo eventuale collisione EASY.\n";
		#endif
		
		if ( ( ( s->type == VERTICAL ) && ( fabs( b->vel.x ) >= EPSILON ) ) || 
		     ( ( s->type == ORIZONTAL ) && ( fabs( b->vel.y ) >= EPSILON ) ) ) {
			type = EASY;
			ret = true;
		} else ret = false;
	} else {
		#if DEBUG > 1
		cout << "Controllo eventuale collisione HARD.\n";
		#endif
		
		if ( s->type == VERTICAL ) {
			//s->a.print();
			if ( bb.pos.y < s->beg.y ) d = &(s->a);
			else d = &(s->b);
		} else {
			//s->a.print();
			if ( bb.pos.x < s->beg.x ) d = &(s->a);
			else d = &(s->b);
		}
		
		//d->print();
		//b->print();
		// TODO
		c.set( d, b, dt );
		ret = c.isCollision();
		if ( ret ) type = HARD;
	}
	
	
	#if DEBUG > 1
	if ( ret )
		cout << "Ci sara' collisione.\n";
	else	cout << "Non ci sara' collisione.\n";
	#endif
	
	return ret;
}



void SideCollision::processCollision ( ) {
	Side* s = (Side*) o;
	Ball* b = (Ball*) p;	// pallina vera
	
	#if DEBUG > 1
	cout << "Processo la collisione col bordo...\n";
	#endif
	
	if ( type == EASY ) {
		// le coordinate sono gia' in bb
		b->pos = bb.pos;
		
		// semplicemente cambio di segno la componente che ha urtato (riflessione)
		if ( s->type == ORIZONTAL )
			b->vel.y = -bb.vel.y;
		else	b->vel.x = -bb.vel.x;
	} else c.processCollision( STATIC );
}




/*
bool HoleCollision::isCollision ( ) {
	return true;
}





void HoleCollision::processCollision ( ) {
	Ball* b = (Ball*) p;
	
	b->out = true;	
}
*/



