#include "stdafx.h"
#include "SceneIO.h"
#include "Scene.h"
#include "RenderSphere.h"
#include "RenderCircle.h"
/*
void DSceneIO::Load(DScene& scene, System::String^ filename)
{
	DScene::GetScene().Reset();

	System::Xml::XmlReaderSettings^ settings = gcnew System::Xml::XmlReaderSettings();
	settings->IgnoreComments = true;
	settings->IgnoreWhitespace = true;
	System::Xml::XmlReader^ reader = System::Xml::XmlReader::Create(filename, settings);
	reader->Read();
	if (reader->Name!="SCENE")
	{
		return;
	}
	reader->Read();
	while(reader->Name!="SCENE")
	{
		if (reader->Name!="OBJECT")
		{
			return;
		}
		LoadObject(scene, reader);
		reader->Read(); // skip end tag
	}
	reader->Close();
}

void DSceneIO::Save(DScene& scene, System::String^ filename)
{
	System::Xml::XmlWriterSettings ^settings = gcnew System::Xml::XmlWriterSettings;
	settings->Indent = true;
	settings->IndentChars = ("\t");
	settings->OmitXmlDeclaration = true;

	System::Xml::XmlWriter^ writer = System::Xml::XmlWriter::Create(filename, settings);
	   
	writer->WriteStartElement("SCENE");
	
	for (int i=0; i<scene.GetNumLights(); i++)
	{
		scene.GetLight(i).Save(writer);
	}

	for (int i=0; i<scene.GetNumRenderObjects(); i++)
	{
		scene.GetRenderObject(i).Save(writer);
	}
	scene.GetCamera().Save(writer);
	scene.GetGlobalLighting().Save(writer);
	
	writer->WriteEndElement();
	writer->Close();
}

void DSceneIO::LoadObject(DScene& scene, System::Xml::XmlReader^ reader)
{
	reader->MoveToAttribute("type");
	if (reader->Value=="DRenderSphere")
	{
		DRenderSphere *sphere = new DRenderSphere();
		sphere->Load(reader);
		scene.AddRenderObject(sphere);
	}
	if (reader->Value=="DRenderCircle")
	{
		DRenderCircle *circle = new DRenderCircle();
		circle->Load(reader);
		scene.AddRenderObject(circle);
	}
	if (reader->Value=="DLight")
	{
		DLight light;
		light.Load(reader);
		scene.AddLight(light);
	}
	if (reader->Value=="DCamera")
	{
		DCamera camera;
		camera.Load(reader);
		scene.AddCamera(camera);
	}
	if (reader->Value=="DGlobalLighting")
	{
		DGlobalLighting globalLighting;
		globalLighting.Load(reader);
		scene.AddGlobalLighting(globalLighting);
	}
}

void DSceneIO::Load(DColour& out_colour, System::Xml::XmlReader^ reader)
{
	reader->MoveToAttribute(1);
	array<Char>^chars = {',',', '};
	array<String^>^ split = reader->Value->Split( chars );
	IEnumerator^ myEnum = split->GetEnumerator();
	myEnum->MoveNext();
	out_colour.mRed = System::Convert::ToSingle(myEnum->Current); 
	myEnum->MoveNext();
	out_colour.mGreen = System::Convert::ToSingle(myEnum->Current); 
	myEnum->MoveNext();
	out_colour.mBlue = System::Convert::ToSingle(myEnum->Current); 
}

void DSceneIO::Load(DVector3& out_vector3, System::Xml::XmlReader^ reader)
{
	reader->MoveToAttribute(1);
	array<Char>^chars = {',',', '};
	array<String^>^ split = reader->Value->Split( chars );
	IEnumerator^ myEnum = split->GetEnumerator();
	myEnum->MoveNext();
	out_vector3[0] = System::Convert::ToSingle(myEnum->Current); 
	myEnum->MoveNext();
	out_vector3[1] = System::Convert::ToSingle(myEnum->Current); 
	myEnum->MoveNext();
	out_vector3[2] = System::Convert::ToSingle(myEnum->Current); 
}

void DSceneIO::Load(float& out_float, System::Xml::XmlReader^ reader)
{
	reader->MoveToAttribute(1);
	out_float = System::Convert::ToSingle(reader->Value);
}

void DSceneIO::Load(int& out_int, System::Xml::XmlReader^ reader)
{
	reader->MoveToAttribute(1);
	out_int = System::Convert::ToInt32(reader->Value);
}

void DSceneIO::Load(std::wstring& out_wstring, System::Xml::XmlReader^ reader)
{
	reader->MoveToAttribute(1);
	using System::IntPtr;
	using System::Runtime::InteropServices::Marshal;
	const wchar_t* chars = (const wchar_t*)(Marshal::StringToHGlobalUni(reader->Value)).ToPointer();
	out_wstring = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

void DSceneIO::Save(System::String^ elementName, DColour& in_colour, System::Xml::XmlWriter^ writer)
{
	writer->WriteStartElement("PARAM");
	writer->WriteStartAttribute("type");
	writer->WriteValue(elementName);
	System::String^ value = System::String::Concat(in_colour.mRed, ", ", in_colour.mGreen, ", ", in_colour.mBlue);
	writer->WriteAttributeString("value", value);
	writer->WriteEndElement();
}

void DSceneIO::Save(System::String^ elementName, DVector3& in_vector3, System::Xml::XmlWriter^ writer)
{
	writer->WriteStartElement("PARAM");
	writer->WriteStartAttribute("type");
	writer->WriteValue(elementName);
	System::String^ value = System::String::Concat(in_vector3[0], ", ", in_vector3[1], ", ", in_vector3[2]);
	writer->WriteAttributeString("value", value);
	writer->WriteEndElement();
}

void DSceneIO::Save(System::String^ elementName, float& in_float, System::Xml::XmlWriter^ writer)
{
	writer->WriteStartElement("PARAM");
	writer->WriteStartAttribute("type");
	writer->WriteValue(elementName);
	System::String^ value = System::Convert::ToString(in_float);
	writer->WriteAttributeString("value", value);
	writer->WriteEndElement();
}

void DSceneIO::Save(System::String^ elementName, int& in_int, System::Xml::XmlWriter^ writer)
{
	writer->WriteStartElement("PARAM");
	writer->WriteStartAttribute("type");
	writer->WriteValue(elementName);
	System::String^ value = System::Convert::ToString(in_int);
	writer->WriteAttributeString("value", value);
	writer->WriteEndElement();
}

void DSceneIO::Save(System::String^ elementName, std::wstring& in_wstring, System::Xml::XmlWriter^ writer)
{
	writer->WriteStartElement("PARAM");
	writer->WriteStartAttribute("type");
	writer->WriteValue(elementName);
	System::String^ value = gcnew System::String(in_wstring.c_str());
	writer->WriteAttributeString("value", value);
	writer->WriteEndElement();
}
*/