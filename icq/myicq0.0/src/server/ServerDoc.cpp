// ServerDoc.cpp : implementation of the CServerDoc class
//

#include "stdafx.h"
#include "Server.h"

#include "ServerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerDoc

IMPLEMENT_DYNCREATE(CServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CServerDoc, CDocument)
	//{{AFX_MSG_MAP(CServerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerDoc construction/destruction

CServerDoc::CServerDoc()
{
	// TODO: add one-time construction code here

}

CServerDoc::~CServerDoc()
{
}

BOOL CServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	((CEditView*)m_viewList.GetHead())->SetWindowText(NULL);

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CServerDoc serialization

void CServerDoc::Serialize(CArchive& ar)
{
	// CEditView contains an edit control which handles all serialization
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CServerDoc diagnostics

#ifdef _DEBUG
void CServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CServerDoc commands
