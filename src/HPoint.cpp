// HPoint.cpp

#include "stdafx.h"
#include "HPoint.h"
#include "PropertyVertex.h"
#include "../tinyxml/tinyxml.h"

static unsigned char cross16[32] = {0x80, 0x01, 0x40, 0x02, 0x20, 0x04, 0x10, 0x08, 0x08, 0x10, 0x04, 0x20, 0x02, 0x40, 0x01, 0x80, 0x01, 0x80, 0x02, 0x40, 0x04, 0x20, 0x08, 0x10, 0x10, 0x08, 0x20, 0x04, 0x40, 0x02, 0x80, 0x01};
static unsigned char cross16_selected[32] = {0xc0, 0x03, 0xe0, 0x07, 0x70, 0x0e, 0x38, 0x1c, 0x1c, 0x38, 0x0e, 0x70, 0x07, 0xe0, 0x03, 0xc0, 0x03, 0xc0, 0x07, 0xe0, 0x0e, 0x70, 0x1c, 0x38, 0x38, 0x1c, 0x70, 0x0e, 0xe0, 0x07, 0xc0, 0x03};

wxIcon* HPoint::m_icon = NULL;

HPoint::~HPoint(void)
{
}

HPoint::HPoint(const gp_Pnt &p, const HeeksColor* col)
{
	m_p = p;
	color = *col;
}

HPoint::HPoint(const HPoint &p)
{
	operator=(p);
}

const HPoint& HPoint::operator=(const HPoint &b)
{
	HeeksObj::operator=(b);
	m_p = b.m_p;
	color = b.color;
	return *this;
}

void HPoint::glCommands(bool select, bool marked, bool no_color)
{
	if(!no_color){
		wxGetApp().glColorEnsuringContrast(color);
	}
	GLfloat save_depth_range[2];
	if(marked){
		glGetFloatv(GL_DEPTH_RANGE, save_depth_range);
		glDepthRange(0, 0);
	}
	glRasterPos3d(m_p.X(), m_p.Y(), m_p.Z());
	glBitmap(16, 16, 8, 8, 10.0, 0.0, marked ? cross16_selected : cross16);
	if(marked){
		glDepthRange(save_depth_range[0], save_depth_range[1]);
	}
}

void HPoint::GetBox(CBox &box)
{
	box.Insert(m_p.X(), m_p.Y(), m_p.Z());
}

HeeksObj *HPoint::MakeACopy(void)const
{
	HPoint *new_object = new HPoint(*this);
	return new_object;
}

wxIcon* HPoint::GetIcon()
{
	if(m_icon == NULL)
	{
		wxString exe_folder = wxGetApp().GetExeFolder();
		m_icon = new wxIcon(exe_folder + _T("/icons/point.png"), wxBITMAP_TYPE_PNG);
	}
	return m_icon;
}

bool HPoint::ModifyByMatrix(const double *m)
{
	gp_Trsf mat = make_matrix(m);
	m_p.Transform(mat);
	return false;
}

void HPoint::GetGripperPositions(std::list<double> *list, bool just_for_endof)
{
	if(just_for_endof)
	{
		list->push_back(0);
		list->push_back(m_p.X());
		list->push_back(m_p.Y());
		list->push_back(m_p.Z());
	}
}

static void on_set_point(const gp_Pnt &vt, HeeksObj* object){
	((HPoint*)object)->m_p = vt;
	wxGetApp().Repaint();
}

void HPoint::GetProperties(std::list<Property *> *list)
{
	__super::GetProperties(list);

	list->push_back(new PropertyVertex(_T("position"), m_p, this, on_set_point));
}

bool HPoint::GetStartPoint(double* pos)
{
	extract(m_p, pos);
	return true;
}

bool HPoint::GetEndPoint(double* pos)
{
	extract(m_p, pos);
	return true;
}

void HPoint::WriteXML(TiXmlElement *root)
{
	TiXmlElement * element;
	element = new TiXmlElement( "Point" );
	root->LinkEndChild( element );  
	element->SetAttribute("col", color.COLORREF_color());
	element->SetDoubleAttribute("x", m_p.X());
	element->SetDoubleAttribute("y", m_p.Y());
	element->SetDoubleAttribute("z", m_p.Z());
	WriteBaseXML(element);
}


//static
HeeksObj* HPoint::ReadFromXMLElement(TiXmlElement* pElem)
{
	gp_Pnt p;
	HeeksColor c;

	// get the attributes
	for(TiXmlAttribute* a = pElem->FirstAttribute(); a; a = a->Next())
	{
		std::string name(a->Name());
		if(name == "col"){c = HeeksColor(a->IntValue());}
		else if(name == "x"){p.SetX(a->DoubleValue());}
		else if(name == "y"){p.SetY(a->DoubleValue());}
		else if(name == "z"){p.SetZ(a->DoubleValue());}
	}

	HPoint* new_object = new HPoint(p, &c);
	new_object->ReadBaseXML(pElem);

	return new_object;
}

