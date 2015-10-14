#include "Utils.h"

#include <fstream>

void rotateLocal_noTranslation(gk::Transform& m, gk::Vector axis, float angle)
{
	float tr1[4][4] = { {0} };
	for(int i=0; i<3; i++)
	for(int j=0; j<4; j++)
	tr1[i][j] = m.matrix().m[i][j];
	tr1[3][3] = 1;

	gk::Transform tmp(tr1);
	tmp = gk::Rotate(angle, axis) * tmp;
	for(int i=0; i<3; i++)
	for(int j=0; j<4; j++)
		tr1[i][j] = tmp.matrix().m[i][j];
	tr1[3][3] = 1;
	tr1[0][3] = m.matrix().m[0][3];
	tr1[1][3] = m.matrix().m[1][3];
	tr1[2][3] = m.matrix().m[2][3];

	m = gk::Transform(tr1);
}
void rotateGlobal_noTranslation(gk::Transform& m, gk::Vector axis, float angle)
{
	float tr1[4][4] = { {0} };
	for(int i=0; i<3; i++)
	for(int j=0; j<4; j++)
	tr1[i][j] = m.matrix().m[i][j];
	tr1[3][3] = 1;

	gk::Transform tmp(tr1);
	tmp = tmp * gk::Rotate(angle, axis);
	for(int i=0; i<3; i++)
	for(int j=0; j<4; j++)
	tr1[i][j] = tmp.matrix().m[i][j];
	tr1[3][3] = 1;
	tr1[0][3] = m.matrix().m[0][3];
	tr1[1][3] = m.matrix().m[1][3];
	tr1[2][3] = m.matrix().m[2][3];

	m = gk::Transform(tr1);
}

void normalizePlane(float* plane)
{
	float lenght = sqrt(plane[0]*plane[0] + plane[1]*plane[1] + plane[2]*plane[2] );
	plane[0] *= lenght;
	plane[1] *= lenght;
	plane[2] *= lenght;
	plane[3] *= lenght;
}
void buildFrustum(float* frustum, gk::Matrix4x4 projection)
{
	//left
	frustum[0] = projection.m[0][3] + projection.m[0][0];
	frustum[1] = projection.m[1][3] + projection.m[1][0];
	frustum[2] = projection.m[2][3] + projection.m[2][0];
	frustum[3] = projection.m[3][3] + projection.m[3][0];
	normalizePlane(frustum);

	//right
	frustum[4] = projection.m[0][3] - projection.m[0][0];
	frustum[5] = projection.m[1][3] - projection.m[1][0];
	frustum[6] = projection.m[2][3] - projection.m[2][0];
	frustum[7] = projection.m[3][3] - projection.m[3][0];
	normalizePlane(frustum + 4);

	// top
	frustum[8] = projection.m[0][3] + projection.m[0][1];
	frustum[9] = projection.m[1][3] + projection.m[1][1];
	frustum[10] = projection.m[2][3] + projection.m[2][1];
	frustum[11] = projection.m[3][3] + projection.m[3][1];
	normalizePlane(frustum + 8);

	// bottom
	frustum[12] = projection.m[0][3] - projection.m[0][1];
	frustum[13] = projection.m[1][3] - projection.m[1][1];
	frustum[14] = projection.m[2][3] - projection.m[2][1];
	frustum[15] = projection.m[3][3] - projection.m[3][1];
	normalizePlane(frustum + 12);

	// near
	frustum[16] = projection.m[0][3] + projection.m[0][2];
	frustum[17] = projection.m[1][3] + projection.m[1][2];
	frustum[18] = projection.m[2][3] + projection.m[2][2];
	frustum[19] = projection.m[3][3] + projection.m[3][2];
	normalizePlane(frustum + 16);

	// far
	frustum[20] = projection.m[0][3] - projection.m[0][2];
	frustum[21] = projection.m[1][3] - projection.m[1][2];
	frustum[22] = projection.m[2][3] - projection.m[2][2];
	frustum[23] = projection.m[3][3] - projection.m[3][2];
	normalizePlane(frustum + 20);
}

void writeMatrix(std::ofstream& file, const gk::Matrix4x4& mat)
{
	#define M44(m, r, c) m[r][c]
	
	file <<	M44(mat.m, 0, 0) << " " << M44(mat.m, 0, 1) << " " << M44(mat.m, 0, 2) << " " << M44(mat.m, 0, 3) << std::endl;
	file <<	M44(mat.m, 1, 0) << " " << M44(mat.m, 1, 1) << " " << M44(mat.m, 1, 2) << " " << M44(mat.m, 1, 3) << std::endl;
	file <<	M44(mat.m, 2, 0) << " " << M44(mat.m, 2, 1) << " " << M44(mat.m, 2, 2) << " " << M44(mat.m, 2, 3) << std::endl;
	file <<	M44(mat.m, 3, 0) << " " << M44(mat.m, 3, 1) << " " << M44(mat.m, 3, 2) << " " << M44(mat.m, 3, 3) << std::endl;
	file << std::endl;
	
	#undef M44
}

void readMatrix(std::ifstream& file, gk::Matrix4x4& mat)
{
	#define M44(m, r, c) m[r][c]

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
		{
			file >> M44(mat.m, i, j);
		}

	#undef M44
}

void write_viewPoint()
{
	std::ofstream myfile;
	myfile.open (DATA_PATH(VIEWPOINT_FILE));
	
	writeMatrix(myfile, Parameters::getInstance()->g_geometry.affine.matrix());
	printf("Wrote to %s\n", DATA_PATH(VIEWPOINT_FILE));

	myfile.close();
}

void load_viewPoint(char* file)
{
	std::ifstream myfile;
	if( file == NULL )
		myfile.open (DATA_PATH(VIEWPOINT_FILE));
	else
		myfile.open (file);

	gk::Matrix4x4 mat;
	readMatrix(myfile, mat);
	
	Parameters::getInstance()->g_geometry.affine = gk::Transform(mat);
	printf("Read from %s\n", DATA_PATH(VIEWPOINT_FILE));

	myfile.close();
}

void write_quatPoint(QuaternionCamera& qc)
{
	std::ofstream myfile;
	myfile.open (DATA_PATH(QUATPOINT_FILE));
	
	qc.write(myfile);
	
	printf("Wrote to %s\n", DATA_PATH(QUATPOINT_FILE));

	myfile.close();
}

void load_quatPoint(QuaternionCamera& qc, char* file)
{
	std::ifstream myfile;
	if( file == NULL )
		myfile.open (DATA_PATH(QUATPOINT_FILE));
	else
		myfile.open (file);

	qc.read(myfile);
	
	printf("Read from %s\n", DATA_PATH(QUATPOINT_FILE));

	myfile.close();
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}


//**** BACKUP ****//

/*
	void init_crack_test()
	{
		m_debug_program= gk::createProgram(SHADER_PATH("passthrough.glsl"));
		long unsigned int nb_vert = 1 << 29;
		m_edges= new gk::GLBasicMesh(GL_LINES, nb_vert);
		m_edges->createBuffer(0, 3, GL_FLOAT, nb_vert * sizeof(gk::Vec3), NULL, GL_STREAM_DRAW);
		m_edges->createBuffer(1, 3, GL_FLOAT, nb_vert * sizeof(gk::Vec3), NULL, GL_STREAM_DRAW);
	}
	
	void run_crack_test(int nb_triangles_regular, int nb_triangles_transition)
	{
                //Fills triangle buffers from the GPU
		std::vector<gk::Vec3> points(0, nb_triangles_regular*3 + nb_triangles_transition*3);
		{
			std::vector<gk::HPoint> hpoints(nb_triangles_regular*3);
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_TRIANGULATION]);
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, hpoints.size() * sizeof(gk::HPoint), &hpoints.front());

			for(unsigned int i= 0; i < hpoints.size(); i++)
				points.push_back( hpoints[i].project() );
		}
		
		{
			std::vector<gk::HPoint> hpoints(nb_triangles_transition*3);
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_TRIANGULATION_TR]);
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, hpoints.size() * sizeof(gk::HPoint), &hpoints.front());

			for(unsigned int i= 0; i < hpoints.size(); i++)
				points.push_back( hpoints[i].project() );
		}

                //Builds a Mesh object with the edges of those triangles
		gk::MeshData mesh;
		gk::buildIndex(points, mesh);
		gk::buildFaces(mesh);
                
                unsigned int edge_counter= 0;
                unsigned int edge_color_counter = 0;
                std::vector<gk::Vec3> colors;

                // Find edges with no adjacent face
		glBindBuffer(GL_ARRAY_BUFFER, m_edges->buffers[0]->name);
		gk::Vec3 *map= (gk::Vec3 *) glMapBufferRange(GL_ARRAY_BUFFER, 0, m_edges->buffers[0]->length, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
		for(unsigned int i= 0; i < mesh.afaces.size(); i++)
		{
			assert(mesh.faces[i].n == 3);
			for(unsigned int k= 0; k < 3; k++)
			{
				map[edge_counter++]= mesh.positions[mesh.faces[i].vertex[k].position];
				map[edge_counter++]= mesh.positions[mesh.faces[i].vertex[(k +1) % mesh.faces[i].n].position];
			}
		}
		glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, edge_counter * sizeof(gk::Vec3));
		glUnmapBuffer(GL_ARRAY_BUFFER);

                // Sets different colors for edges with/without neighbours
		glBindBuffer(GL_ARRAY_BUFFER, m_edges->buffers[1]->name);
		gk::Vec3 *map_color= (gk::Vec3 *) glMapBufferRange(GL_ARRAY_BUFFER, 0, m_edges->buffers[1]->length, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
		for(unsigned int i= 0; i < mesh.afaces.size(); i++)
		{
			assert(mesh.faces[i].n == 3);
			for(unsigned int k= 0; k < 3; k++)
			{
				if(mesh.afaces[i].faces[k] < 0)
				{
					map_color[edge_color_counter++] = gk::Vec3(1, 0, 0);
					map_color[edge_color_counter++] = gk::Vec3(1, 0, 0);
				}
				else
				{
					map_color[edge_color_counter++] = gk::Vec3(0, 0, 1);
					map_color[edge_color_counter++] = gk::Vec3(0, 0, 1);
				}
			}
		}
		glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, edge_color_counter * sizeof(gk::Vec3));
		glUnmapBuffer(GL_ARRAY_BUFFER);

                //Displays the edges
		if(edge_counter > 0)
		{
			glDisable(GL_DEPTH_TEST);

			glUseProgram(m_debug_program->name);
			m_debug_program->uniform("mvpMatrix")= Parameters::getInstance()->g_transforms.modelviewprojection.transpose();

			glBindBuffer(GL_ARRAY_BUFFER, m_edges->buffers[0]->name);
			glBindBuffer(GL_ARRAY_BUFFER, m_edges->buffers[1]->name);
			
			m_edges->count= edge_counter;
			m_edges->draw();
				
			glEnable(GL_DEPTH_TEST);
		}
	}
          */

