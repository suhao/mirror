// Copyright (c) 2006, Rodrigo Braz Monteiro
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Aegisub Group nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Aegisub Project http://www.aegisub.org/

#include "help_button.h"
#include "options.h"

#include <functional>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/checklst.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

namespace {
struct DialogPasteOver {
	wxDialog d;
	wxCheckListBox *ListBox;

	void CheckAll(bool check);

	void OnOK(wxCommandEvent &);
	void OnTimes(wxCommandEvent &);
	void OnText(wxCommandEvent &);

	DialogPasteOver(wxWindow *parent);
};

DialogPasteOver::DialogPasteOver(wxWindow *parent)
: d(parent, -1, _("Select Fields to Paste Over"))
{
	// Label and list sizer
	wxSizer *ListSizer = new wxStaticBoxSizer(wxVERTICAL, &d, _("Fields"));
	ListSizer->Add(new wxStaticText(&d, -1, _("Please select the fields that you want to paste over:")), wxSizerFlags());

	// List box
	wxArrayString choices;
	choices.Add(_("Comment"));
	choices.Add(_("Layer"));
	choices.Add(_("Start Time"));
	choices.Add(_("End Time"));
	choices.Add(_("Style"));
	choices.Add(_("Actor"));
	choices.Add(_("Margin Left"));
	choices.Add(_("Margin Right"));
	choices.Add(_("Margin Vertical"));
	choices.Add(_("Effect"));
	choices.Add(_("Text"));
	ListBox = new wxCheckListBox(&d, -1, wxDefaultPosition, wxDefaultSize, choices);
	ListSizer->Add(ListBox, wxSizerFlags(0).Expand().Border(wxTOP));

	std::vector<bool> options = OPT_GET("Tool/Paste Lines Over/Fields")->GetListBool();
	if (options.size() != choices.size())
		options.resize(choices.size(), false);

	for (size_t i = 0; i < choices.size(); ++i)
		ListBox->Check(i, options[i]);

	// Top buttons
	wxButton *btn;
	wxSizer *TopButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	TopButtonSizer->Add(btn = new wxButton(&d, -1, _("&All")), wxSizerFlags(1));
	btn->Bind(wxEVT_BUTTON, std::bind(&DialogPasteOver::CheckAll, this, true));
	TopButtonSizer->Add(btn = new wxButton(&d, -1, _("&None")), wxSizerFlags(1));
	btn->Bind(wxEVT_BUTTON, std::bind(&DialogPasteOver::CheckAll, this, false));
	TopButtonSizer->Add(btn = new wxButton(&d, -1, _("&Times")), wxSizerFlags(1));
	btn->Bind(wxEVT_BUTTON, &DialogPasteOver::OnTimes, this);
	TopButtonSizer->Add(btn = new wxButton(&d, -1, _("T&ext")), wxSizerFlags(1));
	btn->Bind(wxEVT_BUTTON, &DialogPasteOver::OnText, this);

	// Buttons
	auto ButtonSizer = d.CreateStdDialogButtonSizer(wxOK | wxCANCEL | wxHELP);
	d.Bind(wxEVT_BUTTON, &DialogPasteOver::OnOK, this, wxID_OK);
	d.Bind(wxEVT_BUTTON, std::bind(&HelpButton::OpenPage, "Paste Over"), wxID_HELP);

	// Main sizer
	wxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	MainSizer->Add(ListSizer,0,wxEXPAND | wxLEFT | wxRIGHT,5);
	MainSizer->Add(TopButtonSizer,0,wxLEFT | wxRIGHT | wxEXPAND,5);
	MainSizer->Add(ButtonSizer,0,wxALL | wxEXPAND,5);
	d.SetSizerAndFit(MainSizer);
	d.CenterOnParent();
}

void DialogPasteOver::OnOK(wxCommandEvent &) {
	std::vector<bool> options;
	for (size_t i = 0; i < ListBox->GetCount(); ++i)
		options.push_back(ListBox->IsChecked(i));
	OPT_SET("Tool/Paste Lines Over/Fields")->SetListBool(std::move(options));

	d.EndModal(wxID_OK);
}

void DialogPasteOver::OnText(wxCommandEvent &) {
	CheckAll(false);
	ListBox->Check(10, true);
}

void DialogPasteOver::OnTimes(wxCommandEvent &) {
	CheckAll(false);
	ListBox->Check(2, true);
	ListBox->Check(3, true);
}

void DialogPasteOver::CheckAll(bool check) {
	for (size_t i = 0; i < ListBox->GetCount(); ++i)
		ListBox->Check(i, check);
}
}

bool ShowPasteOverDialog(wxWindow *parent) {
	return DialogPasteOver(parent).d.ShowModal() == wxID_OK;
}
