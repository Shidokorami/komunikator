#include "../include/Klient.h"
#include "../include/MainFrame.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(Klient);

bool Klient::OnInit() {
    MainFrame* mainFrame = new MainFrame("Klient");
   
    
    mainFrame->SetSize(800, 600);
    return true;
}