/*
Author: QIN Shuo
Date: 2016/7/26
Description:
	Color surface
*/


int CreateColorSurface()
{
	/*
if (m_AtlasFileName.find(".nii") != std::string::npos) // there is no .nii file
{
	auto atlas_reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
	atlas_reader->SetFileName(m_AtlasFileName.c_str());
	atlas_reader->Update();
	this->m_Atlas = vtkSmartPointer<vtkImageData>::New();
	this->m_Atlas = atlas_reader->GetOutput();
	// === Create Color lable surface === //
	SurfaceCreator* labelSurface = new SurfaceCreator(this);
	labelSurface->SetInput(m_Atlas);
	labelSurface->SetResamplingFactor(1);
	labelSurface->SetDiscrete(true);
	labelSurface->SetLargestConnected(false);
	labelSurface->Update();
	auto labelMesh = new Mesh(this);
	labelMesh->SetUniqueName("Label");
	//labelMesh->FollowMatrix(labelImage->GetReslicer(0)->GetMeshTransform()->GetMatrix());
	labelMesh->SetPolyData(labelSurface->GetOutput());

	//====construct a lookup table first
	//1. read colormap
	if (this->ReadColorMap() != 0)
	{
		QMessageBox msgBox;
		if (Global_Language == 1)
			msgBox.setText("找不到ColorMap文件，正在退出....");
		else
			msgBox.setText("Cannot Find Atlas Color Map File\n Exit  ...");

		msgBox.exec();
		exit(0);
	}
	//m_ColorTableSize generated in ReadColorMap function
	//2. construct a color map
	m_ColorTable = vtkSmartPointer<vtkLookupTable>::New();
	m_ColorTable->SetNumberOfTableValues(m_ColorTableSize);
	m_ColorTable->Build();
	// Fill in a few known colors, the rest will be generated if needed
	m_ColorTable->SetTableValue(0, 1, 1, 1, 1);  //White
	for (int i = 0; i < m_ColorTableSize / 3; i++)
	{
		m_ColorTable->SetTableValue(1, float(i) / float(m_ColorTableSize / 3), 0, 0, 1);
	}
	for (int i = m_ColorTableSize / 3; i < m_ColorTableSize * 2 / 3; i++)
	{
		m_ColorTable->SetTableValue(1, 0, float(i - m_ColorTableSize / 3) / float(m_ColorTableSize / 3), 0, 1);
	}
	for (int i = m_ColorTableSize * 2 / 3; i < m_ColorTableSize; i++)
	{
		m_ColorTable->SetTableValue(1, 0, 0, float(i - m_ColorTableSize * 2 / 3) / float(m_ColorTableSize / 3), 1);
	}

	double* range = m_Atlas->GetScalarRange();
	labelMesh->SetColor(m_ColorTable, range[0], range[1]);
	m_ImageAtlas = vtkSmartPointer<vtkPolyData>::New();
	m_ImageAtlas = labelMesh->GetPolyData();

	//// ===== Read brain area and color map =============//
	////read surface
	//vtkSmartPointer< vtkPolyDataReader > surfaceReader = vtkSmartPointer< vtkPolyDataReader >::New();
	//surfaceReader->SetFileName(m_AtlasFileName.c_str());
	//surfaceReader->Update();
	//vtkSmartPointer< vtkPolyData > surface = surfaceReader->GetOutput();
	m_BrainActor = vtkSmartPointer<vtkActor>::New();
	m_BrainActor = labelMesh->GetActor();
}
else
{
	auto brain_reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
	brain_reader->SetFileName(m_AtlasFileName.c_str());
	brain_reader->Update();
	m_ImageAtlas = vtkSmartPointer<vtkPolyData>::New();
	m_ImageAtlas = brain_reader->GetOutput();

	//construct a lookup table first
	m_ColorTable = vtkSmartPointer<vtkLookupTable>::New();
	int tableSize = 116;
	m_ColorTable->SetNumberOfTableValues(tableSize);
	m_ColorTable->Build();
	// Fill in a few known colors, the rest will be generated if needed
	m_ColorTable->SetTableValue(0, 0, 0, 0, 0);  //Black
	for (int i = 0; i < 40; i++)
	{
		m_ColorTable->SetTableValue(1, float(i) / 40.0, 0, 0, 1);
	}
	for (int i = 40; i < 80; i++)
	{
		m_ColorTable->SetTableValue(1, 0, float(i - 40 + 1) / 40.0, 0, 1);
	}
	for (int i = 80; i < 116; i++)
	{
		m_ColorTable->SetTableValue(1, 0, 0, float(i - 80 + 1) / 40.0, 1);
	}
	//visualize
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_ImageAtlas);// (surface)
	mapper->SetLookupTable(m_ColorTable);
	mapper->ScalarVisibilityOn();
	m_BrainActor = vtkSmartPointer<vtkActor>::New();
	m_BrainActor->SetMapper(mapper);
	m_BrainActor->GetProperty()->SetInterpolationToFlat();
}







*/
return 0;
}