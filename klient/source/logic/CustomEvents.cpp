#include "../../include/CustomEvents.h"

wxDEFINE_EVENT(REGISTER_REQUESTED, wxCommandEvent);
wxDEFINE_EVENT(BACK_TO_LOGIN_REQUEST, wxCommandEvent);

void sendDynamicEvent(wxEvtHandler* target, wxEventType eventType){
    wxCommandEvent event(eventType);
    wxPostEvent(target, event);
}