


#include "Billiard.h"
#include <iostream>
#include <wx/string.h>
#include <wx/list.h>
#include <typeinfo>
using namespace std;





void ButtonArrow::touch ( Billiard& b ) {
	status = TOUCHED;
	
	b.scrollStatus = NONE;
}


void ButtonArrow::leave ( Billiard& b ) {
	status = NORMAL;
	
	b.scrollStatus = NONE;
}


void ButtonArrow::press ( Billiard& b ) {
	status = PRESSED;
	
	b.scrollStatus = id;
}



void ButtonArrow::draw ( wxBufferedDC& dc ) {
	wxColor darkGray = wxColor( 63, 63, 63 );
	wxColor inside;
	
	switch ( status ) {
		case NORMAL:
			inside = *wxLIGHT_GREY;
			dc.SetBrush( wxBrush( *wxWHITE ) );
			break;
		case TOUCHED:
			inside = darkGray;
			dc.SetBrush( wxBrush( *wxWHITE ) );
			break;
		case PRESSED:
			inside = *wxWHITE;
			dc.SetBrush( wxBrush( *wxBLACK ) );
			break;
		default: break;	
	}
	
	dc.SetPen ( wxPen( *wxBLACK ) );
	dc.DrawCircle( pos.x, pos.y, BR );
	dc.SetBrush( wxBrush( inside ) );
	dc.SetPen ( wxPen( inside ) );
	dc.DrawPolygon( 3, arrow );
}



bool ButtonArrow::isInside ( const wxRealPoint& p ) {
	return mod2( pos - p ) < BR * BR;
}


ButtonArrow::ButtonArrow ( scrollType id, wxRealPoint pos ) {
	this->id = id;
	this->pos = pos;
	status = NORMAL;
	
	// bruttina come soluzione, ma visto che c'ho solo 4 bottoni...
	switch ( id ) {
		case UP:
			arrow[0].x = pos.x;
			arrow[0].y = pos.y - BRI;
			arrow[1].x = pos.x - BRI;
			arrow[1].y = pos.y + BRI;
			arrow[2].x = pos.x + BRI;
			arrow[2].y = pos.y + BRI;
			break;
		case DOWN:
			arrow[0].x = pos.x;
			arrow[0].y = pos.y + BRI;
			arrow[1].x = pos.x - BRI;
			arrow[1].y = pos.y - BRI;
			arrow[2].x = pos.x + BRI;
			arrow[2].y = pos.y - BRI;
			break;
		case LEFT:
			arrow[0].x = pos.x + BRI;
			arrow[0].y = pos.y - BRI;
			arrow[1].x = pos.x - BRI;
			arrow[1].y = pos.y;
			arrow[2].x = pos.x + BRI;
			arrow[2].y = pos.y + BRI;
			break;
		case RIGHT:
			arrow[0].x = pos.x + BRI;
			arrow[0].y = pos.y;
			arrow[1].x = pos.x - BRI;
			arrow[1].y = pos.y + BRI;
			arrow[2].x = pos.x - BRI;
			arrow[2].y = pos.y - BRI;
			break;
		default: break;	
	}
}















double Billiard::zoom = MINZOOM;


Billiard::~Billiard ( ) {

	clear( );
}




Billiard::Billiard ( wxWindow* w ) : wxControl ( w, -1 ) {
	w->Centre( );
	init( );
}



bool Billiard::ballPositionIsOk ( Ball& b ) {
	// controllo altre palline
	for ( list<Ball>::iterator j = balls.begin(); j != balls.end(); j++ )
		if ( j->collide( b ) ) return false;
	
	for ( uint i = 0; i < sides.size(); i++ )
		if ( sides[i].collide( b ) ) return false;
	
	for ( uint i = 0; i < holes.size(); i++ )
		if ( holes[i].collide( b ) ) return false;
	
	return true;
}



//#if DEBUG
// crea una pallina casuale
void Billiard::randomBall ( int i, uint minW, uint maxW, uint minH, uint maxH ) {
	bool ok;
	Ball b;
	
	do {
		b = Ball ( wxRealPoint( minW + random() % ( maxW - minW ), minH + random() % ( maxH - minH ) ), i );
		ok = true;
	
		for ( list<Ball>::iterator j = balls.begin(); j != balls.end() && ok; j++ ) {
			if ( mod2( b.pos - j->pos ) < 4.0 * REALRADIUS * REALRADIUS )
				ok = false;
		}
	} while ( !ok );
	
	balls.push_front( b );
}


// crea tante palline casuali
void Billiard::randomBalls ( int n, uint minW, uint maxW, uint minH, uint maxH ) {
	for ( int i = n - 1; i >= 0; i-- )
		randomBall( i, minW, maxW, minH, maxH );
}


void Billiard::randomSpeeds ( double max ) {
	for ( list<Ball>::iterator i = balls.begin(); i != balls.end(); i++ ) {
		i->randomSpeed( max );
	}
}
//#endif



void Billiard::positionBalls( int n, uint minW, uint maxW, uint minH, uint maxH ) {
	double yStep = REALRADIUS;
	double xStep = sqrt( 3.0 ) * REALRADIUS;
	
	wxRealPoint pos = wxRealPoint( ( maxW + minW ) * 0.3333, ( maxH + minH ) * 0.5 );
	balls.push_front( Ball( pos, 1 ) ); pos.x -= xStep; pos.y -= yStep;
	balls.push_front( Ball( pos, 2 ) ); pos.y += 2.0 * yStep;
	balls.push_front( Ball( pos, 3 ) ); pos.x -= xStep; pos.y += -3.0 * yStep;
	balls.push_front( Ball( pos, 4 ) ); pos.y += 2.0 * yStep;
	balls.push_front( Ball( pos, 5 ) ); pos.y += 2.0 * yStep;
	balls.push_front( Ball( pos, 6 ) ); pos.x -= xStep; pos.y += -5.0 * yStep;
	balls.push_front( Ball( pos, 7 ) ); pos.y += 2.0 * yStep;
	balls.push_front( Ball( pos, 8 ) ); pos.y += 2.0 * yStep;
	balls.push_front( Ball( pos, 9 ) ); pos.y += 2.0 * yStep;
	balls.push_front( Ball( pos, 10 ) ); pos.x -= xStep; pos.y += -7.0 * yStep;
	balls.push_front( Ball( pos, 11 ) ); pos.y += 2.0 * yStep;
	balls.push_front( Ball( pos, 12 ) ); pos.y += 2.0 * yStep;
	balls.push_front( Ball( pos, 13 ) ); pos.y += 2.0 * yStep;
	balls.push_front( Ball( pos, 14 ) ); pos.y += 2.0 * yStep;
	balls.push_front( Ball( pos, 15 ) );
	
	pos = wxRealPoint( maxW - ( maxW + minW ) * 0.2, ( maxH + minH ) * 0.5 );
	balls.push_front( Ball( pos, 0 ) );
}



bool Billiard::everythingStopped (  ) {
	// controllo solo se nn e' gia' tutto fermo, visto che metto il flag stopped a true
	// solo dopo che ho cliccato (oppure dopo questo ciclo)
	for ( list<Ball>::iterator i = balls.begin(); i != balls.end() && !stopped; i++ ) {
		if ( !( i->isStopped() ) ) return stopped = false;
	}
	
	return stopped = true;
}



void Billiard::init ( ) {
	//inizializzo lo sfondo
	backColor = wxColour( 127, 127, 127, wxALPHA_OPAQUE ); //= *wxDARK_GREY;
	
	//inizializzo il timer
	timer.SetOwner ( this );
	timer.Start ( DT );

	offset = wxRealPoint( 0.0, 0.0 );
	
	sides.push_back( Side( wxRealPoint( ABORDER, BORDER ),
			       wxRealPoint( WHALF - OBORDER, BORDER ), ORIZONTAL, -1.0 ) );
	sides.push_back( Side( wxRealPoint( WHALF + OBORDER, BORDER ),
			       wxRealPoint( W - ABORDER, BORDER ), ORIZONTAL, -1.0 ) );		       
	sides.push_back( Side( wxRealPoint( BORDER, ABORDER ), 
			       wxRealPoint( BORDER, H - ABORDER ), VERTICAL, -1.0 ) );
	sides.push_back( Side( wxRealPoint( W - BORDER, ABORDER ),
			       wxRealPoint( W - BORDER, H - ABORDER ), VERTICAL, +1.0 ) );
	sides.push_back( Side( wxRealPoint( ABORDER, H - BORDER ), 
			       wxRealPoint( WHALF - OBORDER, H - BORDER ), ORIZONTAL, +1.0 ) );
	sides.push_back( Side( wxRealPoint( WHALF + OBORDER, H - BORDER ), 
			       wxRealPoint( W - ABORDER, H - BORDER ), ORIZONTAL, +1.0 ) );
			       
	holes.push_back( wxRealPoint( 0.0, 0.0 ) );
	holes.push_back( wxRealPoint( W, H ) );
	holes.push_back( wxRealPoint( W, 0.0 ) );
	holes.push_back( wxRealPoint( 0.0, H ) );
	holes.push_back( wxRealPoint( WHALF, BORDER - HOLE ) );
	holes.push_back( wxRealPoint( WHALF, H - ( BORDER - HOLE ) ) );
	
	//holes.push_back( Hole( wxRealPoint( 10.0, 10.0 ) ) );
	// aggiungo le palline
	//srandom( getpid( ) );
	//randomBalls( 16, BORDER + RADIUS, W - BORDER - RADIUS, BORDER + RADIUS, H - BORDER - RADIUS );
	//randomSpeeds( 0.3 );
	positionBalls( 1, BORDER + REALRADIUS, W - BORDER - REALRADIUS, BORDER + REALRADIUS, H - BORDER - REALRADIUS );
		
	stopped = false;
	everythingStopped( );
	offset = wxRealPoint( 0.0, 0.0 );
	
	
	scrollStatus = NONE;
	buttons.push_back( ButtonArrow( UP, wxRealPoint( WHALF, 3.0 + BR ) ) );
	buttons.push_back( ButtonArrow( DOWN, wxRealPoint( WHALF, H - 3.0 - BR ) ) );
	buttons.push_back( ButtonArrow( LEFT, wxRealPoint( 3.0 + BR, HHALF ) ) );
	buttons.push_back( ButtonArrow( RIGHT, wxRealPoint( W - 3.0 - BR, HHALF ) ) );
	
	game = PLAYING;
	
	// punti per disegnare la stecca
	cue[2].x = 0.0;
	cue[2].y = CUEHEADH;
	cue[3].x = 0.0;
	cue[3].y = -CUEHEADH;
	cue[0].x = CUEHEADL;
	cue[0].y = -CUEHEADH;
	cue[1].x = CUEHEADL;
	cue[1].y = CUEHEADH;
	cue[5].x = CUELEN;
	cue[5].y = CUEHEADH * 1.8;
	cue[4].x = CUELEN;
	cue[4].y = -CUEHEADH * 1.8;
}


void Billiard::clear ( ) {
	balls.clear( );
}





/* Qui ovviamente comincia il bordello. Assunzioni:
 * 1) Considero che ogni pallina fino, all'istante DT, si muova di moto uniforme
 * 
 * Praticamente procedo per urti. Se trovo un urto prima dell'istante finale (dt) allora
 * aggiorno tutto il sistema all'istante immediatamente successivo all'urto.
 * Itero il procedimento finche nn ci sono piu' urti prima di DT, e allora posso aggiornare
 * tutto tranquillamente all'istante finale.
 */
void Billiard::calcPhysix ( ) {
	wxRealPoint p, dx;
	bool found;
	double dt = DT, min, tdt;
	//BallCollision best, tmp;
	BallCollision tmp1, tmp2;
	SideCollision stmp1, stmp2;
	const type_info& b = (typeid(Ball));
	
	
	if ( everythingStopped( ) ) return;
	
	if ( balls.begin()->num == 0 ) game = PLAYING;
	
	do {
		Collision* best = NULL;
		found = false;
		min = dt;
		
		
		// cerco urti
		#if DEBUG
		cout << "INIZIO RICERCA (dt residuo = " << dt << ")\n";
		#endif
		for ( list<Ball>::iterator i = balls.begin(); i != balls.end(); i++ ) {
			//if ( i->isStopped() ) continue;
			
			// per le buche nn definisco una classe Collision
			// visto che la gestione e' abbastanza banale
			for ( uint j = 0; j < holes.size(); j++ ) {
				if ( holes[j].collide( *i ) ) {
					#if DEBUG
					cout << "PALLINA IN BUCA" << endl;
					#endif
					list<Ball>::iterator j = i;
					i--;
					if ( j->num == 0 ) game = POSITION;
					balls.erase( j );
					continue;
				}
			}
			
			for ( list<Ball>::iterator j = i; j != balls.end(); j++ ) {
				if ( i == j ) continue;
				
				tdt = min;
				if ( j->collide( *i, tdt ) ) {
					if ( tdt < min ) {
						#if DEBUG
						cout << "URTO FRA PALLINE a t = " << tdt << endl;
						#endif
						// guardo se e' un urto vero o semplicemente le palline si stanno
						// toccando (vabe' ci sono vari casi)
						tmp2.set( &(*i), &(*j), tdt );
						
						if ( tmp2.isCollision() ) {
							#if DEBUG
							cout << "E' IL MIGLIORE TROVATO FINORA\n";
							#endif
							tmp1 = tmp2;
							best = &tmp1;
							min = tdt;
							found = true;
						} 
						#if DEBUG
						else 	cout << "IN REALTA NON E' UN URTO\n";
						#endif
					}
				}
			}
			
			for ( uint j = 0; j < sides.size(); j++ ) {
				tdt = min;
				
				if ( sides[j].collide( *i, tdt ) ) {				
					if ( tdt < min ) {
						#if DEBUG
						cout << "URTO CON SPONDA a t = " << tdt << endl;
						#endif
						
						stmp2.set( &sides[j], &(*i), tdt );
					
						if ( stmp2.isCollision( ) ) {
							#if DEBUG
							cout << "E' IL MIGLIORE TROVATO FINORA\n";
							#endif
							stmp1 = stmp2;
							best = &stmp1;
							found = true;
							min = tdt;
						}
						#if DEBUG
						else 	cout << "IN REALTA NON E' UN URTO\n";
						#endif
					}
				}
			}
		}
		#if DEBUG
		cout << "FINE RICERCA" << endl << endl;
		#endif
		
		if ( found ) {
			dt -= min;
		
		
			// aggiorno il sistema all'instante min (se ho trovato un urto)
		
			//cout << "Urto fra pallina e pallina entro " << min << ".\n";
			//cout << "Urto fra pallina e sponda entro " << min << ".\n";
			
			best->processCollision();
			
			// aggiorno tutte le altre palline
			for ( list<Ball>::iterator i = balls.begin(); i != balls.end(); i++ ) {
				// nn aggiorno le palline che hanno urtato (sono gia' al lor posto)
				if ( &(*i) == best->p )
					continue;
				
				if ( typeid(*(best->o)) == b )
					if ( &(*i) == best->o ) continue;
		
				i->updateAfter( min );
			}
		}
	} while ( found );
	
	#if DEBUG
	cout << "PROCESSATI TUTTI GLI URTI\n";
	cout << "Palline ancora in gioco: " << balls.size() << endl;
	#endif
	
	
	
	
	
	
	// aggiorno tutte le altre palline
	for ( list<Ball>::iterator i = balls.begin(); i != balls.end(); i++ )
		if ( ! i->isStopped() )
			i->updateAfter( dt );
}



void Billiard::drawTable ( wxBufferedDC& dc ) {
	for ( uint i = 0; i < holes.size(); i++ )
		holes[i].draw( dc );
	
	for ( uint i = 0; i < sides.size(); i++ )
		sides[i].draw( dc );
	
}


void Billiard::drawBalls ( wxBufferedDC& dc ) {
	// setto il colore per disegnare
	dc.SetBrush( wxBrush( *wxBLACK ) );
	// setto il font
	wxFont font = *wxSMALL_FONT;
	font.SetPointSize( font.GetPointSize() * zoom );
	dc.SetFont ( font );

	for ( list<Ball>::iterator i = balls.begin(); i != balls.end(); i++ )
		i->draw( dc );
}


// in realta' disegna la stecca oppure una pallina se la bianca e' andata in buca
void Billiard::drawCue ( wxBufferedDC& dc ) {
	dc.SetBrush( wxBrush( *wxWHITE ) );
	
	#if NATIVEZOOMING
	double z = 1.0;
	#else
	double z = Billiard::zoom;
	#endif
	
	if ( everythingStopped( ) ) {
		if ( game == PLAYING ) {
			dc.SetBrush( wxBrush( *wxBLACK ) );
			//dc.DrawLine( balls.begin()->pos.x * z, balls.begin()->pos.y * z, mouseAbsolute.x * z, mouseAbsolute.y * z );balls.begin()->pos
			
			wxRealPoint tmp = z * ( cueDistance * cueDir + balls.begin()->pos );
			dc.DrawPolygon( 4, cuee + 2, tmp.x, tmp.y );			
			dc.SetBrush( wxBrush( *wxWHITE ) );
			dc.DrawPolygon( 4, cuee, tmp.x, tmp.y );
			//dc.DrawCircle( tmp.x * z, tmp.y * z, 0.5 * RADIUS * z );
		} else {
			dc.SetBrush( wxBrush( *wxWHITE ) );
			dc.DrawCircle( mouseAbsolute.x * z, mouseAbsolute.y * z, RADIUS * z );
		}
	}
}

void Billiard::drawButtons ( wxBufferedDC& dc ) {
	for ( uint i = 0; i < buttons.size(); i++ )
		buttons[i].draw( dc );
}




















void Billiard::Command ( wxCommandEvent&  event ) {
	#ifdef DEBUG
	cerr << "Command" << endl;
	#endif
}



void Billiard::scroll ( ) {
	double step = SCROLLSTEP * zoom;
	
	switch ( scrollStatus ) {
		case UP: offset.y += step; break;
		case DOWN: offset.y -= step; break;
		case LEFT: offset.x += step; break;
		case RIGHT: offset.x -= step; break;
		case NONE:
		default: break;	
	}
	
	// non voglio vedere fuori dal tavolo
	if ( offset.x > 0.0 ) offset.x = 0.0;
	if ( offset.y > 0.0 ) offset.y = 0.0;
	if ( offset.x < W * ( 1.0 -zoom ) ) offset.x = W * ( 1.0 -zoom );
	if ( offset.y < H * ( 1.0 -zoom ) ) offset.y = H * ( 1.0 -zoom );
}




void Billiard::onMouseClick ( wxMouseEvent& e ) {
#ifdef DEBUG
	cerr << "Mouse Click" << endl;
#endif
	mouse = wxRealPoint( e.GetX( ), e.GetY( ) );
	e.Skip( ); // a quanto pare se nn faccio cosi' nn mi arrivano eventi tastiera
	
	
	
	// se sono su un bottone lo pigio e non faccio altro
	if ( onButton ) {
		active->press( *this );
		return;
	}
	/*for ( uint i = 0; i < buttons.size(); i++ )
		if ( buttons[i].isInside( mouse ) ) {
			buttons[i].press( *this );
			return;
		}*/
			
	if ( everythingStopped( ) ) {
		if ( game == PLAYING ) {		
			wxRealPoint newVel = normalize( balls.begin()->pos - mouseAbsolute );
			balls.begin()->vel = speed * newVel;
			stopped = false;
		} else {
			Ball b ( mouseAbsolute, 0 );
			if ( ballPositionIsOk( b ) ) {
				balls.push_front( b );
				game = PLAYING;
			}
		}
	}
	
	
	//balls.clear();
	//randomBalls( 16, BORDER + RADIUS, W - BORDER - RADIUS, BORDER + RADIUS, H - BORDER - RADIUS );
	//randomSpeeds( 0.3 );
}



void Billiard::onMouseUp ( wxMouseEvent& e ) {
#ifdef DEBUG
	cerr << "Mouse Up" << endl;
#endif
	mouse = wxRealPoint( e.GetX( ), e.GetY( ) );
	
	if ( onButton ) {
		active->touch( *this );
		return;
	}
	/*for ( uint i = 0; i < buttons.size(); i++ )
		if ( buttons[i].isInside( mouse ) ) {
			buttons[i].touch();
			scrollStatus = NONE;
			break;
		}*/
}



void Billiard::onMouseMove ( wxMouseEvent& e ) {
#ifdef DEBUG
	cerr << "Mouse Move" << endl;
#endif
	// mi salvo le coordinate del mouse (anche assolute) per le altre funzioni
	mouse = wxRealPoint( e.GetX( ), e.GetY( ) );
	mouseAbsolute = 1.0 / zoom * ( mouse - offset );
	
	
	// calcolo la distanza della stecca, necessaria per quando viene sparata la pallina
	cueDistance = sqrt( mod2( balls.begin()->pos - mouseAbsolute ) );
	if ( cueDistance > MAXCUEDISTANCE ) cueDistance = MAXCUEDISTANCE;
	speed = -0.03 * cueDistance;
	if ( cueDistance < RADIUS ) cueDistance = RADIUS;
	
	// ruoto di nuovo la stecca, ma nn c'e' un modo piu' sintetico?
	cueDir = normalize( balls.begin()->pos - mouseAbsolute );
	wxRealPoint pdir = perpendicular( cueDir );
			
	// veramente scandaloso...
	for ( int i = 0; i < 6; i++ ) {
		cuee[i].x = changeCoordinates( cue[i], cueDir, pdir ).x * zoom;
		cuee[i].y = changeCoordinates( cue[i], cueDir, pdir ).y * zoom;
	}
	
	
	// guardo eventuali correlazioni con i bottoni
	for ( uint i = 0; i < buttons.size() && zoom != 1.0; i++ )
		if ( buttons[i].isInside( mouse ) ) {
			onButton = true;
			active = &buttons[i];
			active->touch( *this );
			return;
		} else	buttons[i].leave( *this );
	
	onButton = false;
}



void Billiard::changeZoom ( bool in, bool mouseZoom ) {
	
	// vecchie coordinate del centro della finestra
	wxRealPoint winCentOld = 1.0 / zoom * ( 0.5 * wxRealPoint( W, H ) - offset );
	
	if ( in ) {
		zoom *= ZOOMSTEP;
	} else 	zoom /= ZOOMSTEP;
	
	// non voglio abbassare lo zoom piu' di un certo livello	
	if ( zoom < MINZOOM ) zoom = MINZOOM;
	
	// mezza finestra in dimensione trasformate
	wxRealPoint winAbsNew = 0.5 / zoom * wxRealPoint( W, H );
	if ( mouseZoom )
		offset = zoom * ( winAbsNew - mouseAbsolute );
	else	offset = zoom * ( winAbsNew - winCentOld );
	
	scroll( );
	// la coordinata assoluta del mouse sara' cambiata quindi la ricalcolo
	mouseAbsolute = 1.0 / zoom * ( mouse - offset );
}



void Billiard::onMouseWheel ( wxMouseEvent& e ) {
#ifdef DEBUG
	cerr << "Mouse Wheel" << endl;
#endif
	
	changeZoom( e.GetWheelRotation( ) > 0, true );
}


void Billiard::onPaint ( wxPaintEvent& e ) {
	
	wxPaintDC dcdraw ( this );
	wxBufferedDC dc ( &dcdraw, GetClientSize( ) );
	
	// disegno tutta la robba
	dc.SetBackground( wxBrush( backColor ) );
	dc.Clear( );
	
	#if NATIVEZOOMING
	dc.SetUserScale( zoom, zoom );
	#endif
	//cout << offset.x << endl;
	dc.SetDeviceOrigin( offset.x, offset.y );
	
	// disegno tutta la robba
	//dc.SetBackground( wxBrush( tableColor ) );
	//dc.Clear( );
	drawTable( dc );
	drawBalls( dc );
	// la stecca la disegno solo se non sono su un bottone, in quanto in questo do'
	// priorita' al bottone
	if ( !onButton ) drawCue( dc );
	
	
	dc.SetDeviceOrigin( 0.0, 0.0 );
	if ( zoom != 1.0 ) drawButtons( dc );
}




void Billiard::onTimer ( wxTimerEvent& e ) {
	
	calcPhysix( );
	scroll( );
	
	
	Refresh( );
}



void Billiard::onSize ( wxSizeEvent& e ) {
#ifdef DEBUG
	cerr << "Size Event" << endl;
#endif
	
}



void Billiard::onKeyUp ( wxKeyEvent& e ) {
	#ifdef DEBUG
	cout << "Key Released" << endl;
	#endif
	
	
	switch ( e.GetKeyCode() ) {
		case WXK_LEFT:
    			buttons[LEFT].leave( *this ); break;
    		case WXK_RIGHT:
    			buttons[RIGHT].leave( *this ); break;
    		case WXK_UP:
    			buttons[UP].leave( *this ); break;
    		case WXK_DOWN:
    			buttons[DOWN].leave( *this ); break;
    		default: break;
	}
	
	if ( onButton ) active->touch( *this );
}


void Billiard::onKeyDown ( wxKeyEvent& e ) {
	#ifdef DEBUG
	cout << "Key Pressed" << endl;
	#endif
	
	
	
	
	switch ( e.GetKeyCode() ) {
		case WXK_LEFT:
    			buttons[LEFT].press( *this ); break;
    		case WXK_RIGHT:
    			buttons[RIGHT].press( *this ); break;
    		case WXK_UP:
    			buttons[UP].press( *this ); break;
    		case WXK_DOWN:
    			buttons[DOWN].press( *this ); break;
    		case WXK_ESCAPE:
    			exit( 0 );
    		case WXK_ADD:
    		case WXK_NUMPAD_ADD:
    			changeZoom( true, false ); break;
    		case WXK_SUBTRACT:
    		case WXK_NUMPAD_SUBTRACT:
    			changeZoom( false, false ); break;
    		default: break;
	}
	
	//if ( zoom < MINZOOM ) zoom = MINZOOM;
}



















BEGIN_EVENT_TABLE ( Billiard, wxControl )
	//Tastiera
	EVT_KEY_UP ( Billiard::onKeyUp )
	EVT_KEY_DOWN ( Billiard::onKeyDown ) 
	//Mouse
	EVT_MOUSEWHEEL ( Billiard::onMouseWheel )
	EVT_LEFT_DOWN ( Billiard::onMouseClick )
	EVT_LEFT_UP ( Billiard::onMouseUp )
	EVT_MOTION ( Billiard::onMouseMove ) 
	//Window
	EVT_PAINT ( Billiard::onPaint )
	EVT_SIZE ( Billiard::onSize )
	//Timer
	EVT_TIMER ( -1, Billiard::onTimer )
END_EVENT_TABLE ( )




