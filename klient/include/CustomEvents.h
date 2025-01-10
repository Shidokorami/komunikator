#pragma once
#ifndef CUSTOMEVENTS_H
#define CUSTOMEVENTS_H
#include <wx/event.h>


wxDECLARE_EVENT(REGISTER_REQUESTED, wxCommandEvent);
wxDECLARE_EVENT(BACK_TO_LOGIN_REQUEST, wxCommandEvent);

void sendDynamicEvent(wxEvtHandler* target, wxEventType eventType);

#endif