#include <wxPanelType.h>

#include <wxPanelNode.h>
#include <wxFrameMain.h>
#include <opcutils.h>
#include <wx/log.h>

#include <vector>

//(*InternalHeaders(wxPanelType)




//(*IdInit(wxPanelType)








BEGIN_EVENT_TABLE(wxPanelType,wxPanel)
	//(*EventTable(wxPanelType)
	//*)
END_EVENT_TABLE()

using namespace std;

wxPanelType::wxPanelType(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(wxPanelType)










































}

wxPanelType::~wxPanelType()
{
	//(*Destroy(wxPanelType)
	//*)
}

bool wxPanelType::UpdateData(const char *nodePrefix)
{
    this->m_type->IsAbstract(this->chkAbstract->GetValue());
    this->m_type->NoClassGeneration(this->chkNoClassGen->GetValue());

    return this->panelNode->UpdateData(nodePrefix);
}

void wxPanelType::PopulateData(const enum NodeType nodeType)
{
    this->chkAbstract->SetValue(this->m_type->IsAbstract());
    this->chkNoClassGen->SetValue(this->m_type->NoClassGeneration());

    ListOfChildren childrenList;
    ListOfReferences referencesList;
    NodeDesign::Children_optional children(childrenList);
    NodeDesign::References_optional references(referencesList);

    //TODO: Populate own fields here.

    wxString baseType        = OPCUtils::GetName<TypeDesign::BaseType_optional>(this->m_type->BaseType());
    int indexFoundForTypeDef = -1;
    int nodeIndex            = 0;

    //TODO: Make this selection polymorphic
    //      If they have their own panel, let's move this code to their ownself.
    switch (nodeType)
    {
        case NodeTypeObjectType:
            {
               ITERATE_MODELLIST(ObjectType, i, m_model)
               {
                    wxString symName = OPCUtils::GetName<NodeDesign::SymbolicName_optional>(i->SymbolicName());

                    //Find the ObjectType that the Object has typedef'd from.
                    if (    -1 == indexFoundForTypeDef
                         &&  0 != this->m_type->BaseType()
                         &&  0 == symName.compare(baseType) )
                    {
                        indexFoundForTypeDef = nodeIndex;
                        this->CopyParent(&(*i), &children, &references);
                    }
                    ++nodeIndex;
                }
                this->panelNode->PopulateChildren(&children);
                break;
            }

        case NodeTypeVariableType:
            {
                ITERATE_MODELLIST(VariableType, i, m_model)
                {
                    wxString symName = OPCUtils::GetName<NodeDesign::SymbolicName_optional>(i->SymbolicName());


                    //Find the VariableType that the Object has typedef'd from.
                    if (    -1 == indexFoundForTypeDef
                         &&  0 != this->m_type->BaseType()
                         &&  0 == symName.compare(baseType) )
                    {
                        indexFoundForTypeDef = nodeIndex;
                        this->CopyParent(&(*i), &children, &references);
                    }
                    ++nodeIndex;
                }
                this->panelNode->PopulateChildren(&children);
                break;
            }
        case NodeTypeDataType:
            {
                ITERATE_MODELLIST(DataType, i, m_model)
                {
                    wxString symName = OPCUtils::GetName<NodeDesign::SymbolicName_optional>(i->SymbolicName());

                    //Find the VariableType that the Object has typedef'd from.
                    if (    -1 == indexFoundForTypeDef
                         &&  0 != this->m_type->BaseType()
                         &&  0 == symName.compare(baseType))
                    {
                        indexFoundForTypeDef = nodeIndex;
                        this->CopyParent(&(*i), &children, &references);
                    }
                    ++nodeIndex;
                }
                this->panelNode->PopulateChildren(&children);
                break;
            }
        default:
            cerr << "Error: wxPanelType::PopulateData() case:" << nodeType << " not handled.\n";
    }
    //TODO: Other model element type

    this->panelNode->PopulateData();

    this->panelNode->PopulateReferences(&references);
}

void wxPanelType::Init(TypeDesign* type, ModelDesign *model, wxTreeItemId treeItemId, wxFrameMain *mainFrame,
                       bool userOwner, bool hideEnum, bool hideChildren)
{
    this->m_type            = type;
    this->m_model           = model;
    this->m_treeItemId      = treeItemId;
    this->m_mainFrame       = mainFrame;

    this->chkNoClassGen->Enable(userOwner);
    this->chkAbstract->Enable(userOwner);

    this->panelNode->Init(type, model, treeItemId, mainFrame, userOwner, hideEnum, hideChildren);
}

void wxPanelType::CopyParent(TypeDesign *type, NodeDesign::Children_optional *inheritedChildren, NodeDesign::References_optional *inheritedReferences)
{
    //TODO: Add a logic to check for cyclic BaseType where a parent inherits a Child.
    //      Store baseType in a map, if key is there already then we terminate recursion and show message

    this->panelNode->CopyParent(type, inheritedChildren, inheritedReferences);

    //Get the BaseType and search the actual object
    wxString baseType = OPCUtils::GetName<TypeDesign::BaseType_optional>(type->BaseType(), "BaseObjectType");

    //Recursion will stop at the BaseObjectType
    if (baseType.Cmp("BaseObjectType") == 0)
        return;


    TypeDesign *parentType = 0;

    //Search from ObjectType
    for (ModelDesign::ObjectType_iterator i (this->m_model->ObjectType().begin());
         i != this->m_model->ObjectType().end ();
         ++i)
    {
        wxString symName = OPCUtils::GetName<NodeDesign::SymbolicName_optional>(i->SymbolicName());

        //Find the ObjectType that the Object has typedef'd from.
        if (0 == symName.compare(baseType))
        {
            parentType = &(*i);
            break;
        }
    }

    //TODO: Continue searching with the rest of the model element type if parentType is not null.

    //Search from VariableType

    //Search from PropertyType

    //Search from DataType

    //Search from ReferenceType?


    if (parentType != 0)
        this->CopyParent(parentType, inheritedChildren, inheritedReferences);
}