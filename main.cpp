
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/frame.h>
#include "Billiard.h"



class MyApp : public wxApp {
	bool OnInit( );
private:
	Billiard* b;
};

bool MyApp::OnInit ( ) {
	//wxFrame* f = new wxFrame( (wxFrame*) NULL, -1, _T( "Billiard" ), wxPoint( 100, 100 ), wxSize( W, H ) );
	
	wxFrame* f = new wxFrame ( (wxFrame*) NULL, -1, _T( "Billiard" ), wxPoint( 100, 100 ), wxSize( W, H ), wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX) );
	
	
	b = new Billiard ( f );
	
	f->Show( );
	return true;
}




IMPLEMENT_APP(MyApp)

