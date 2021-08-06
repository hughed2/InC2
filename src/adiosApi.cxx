#include "adiosApi.h"

#include <adios2.h>
#include <mpi.h>

template <class T>
class adiosApi_impl {

  adios2::fstream out;
  std::vector<std::string> labels;

  public:
    adiosApi_impl(std::string fn="adios-writer.bp");
    ~adiosApi_impl();

    void flush();

    void addPoints(const T*, const T*, const T*, size_t);
    void addPointData(const std::string &, const T*);
    void addCell(size_t*, size_t, int);

  private:
    size_t numPoints = 0;
    size_t numCells = 0;
    std::vector<std::string> pointDataLabels;
    std::vector<size_t> cells;
};

template <class T>
adiosApi<T>::adiosApi(std::string fn)
{
  impl = new adiosApi_impl<T>(fn);
}

template <class T>
adiosApi_impl<T>::adiosApi_impl(std::string fn)
{
  out = adios2::fstream(fn, adios2::fstream::out, "BP4");
}

template <class T>
adiosApi<T>::~adiosApi()
{
  delete impl;
}

template <class T>
void adiosApi_impl<T>::flush()
{
  if (this->numPoints == 0 or this->numCells == 0)
  {
    std::cerr << "Both points and cells must be provided to write VTK file" << std::endl;
  }

  // We need to add XML for VTK to understand the results
  std::string imageData = R"(
     <?xml version="1.0"?>
     <VTKFile type="UnstructuredGrid" version="0.1" byte_order="LittleEndian">
       <UnstructuredGrid>
         <Piece NumberOfPoints="NumOfVertices" NumberOfCells="NumOfElements">
           <Points>
             <DataArray Name="vertices" />
           </Points>
           <Cells>
             <DataArray Name="types" />
             <DataArray Name="connectivity" />
           </Cells>)";

  if (!pointDataLabels.empty())
  {
    imageData += "<PointData>";
    std::vector<std::string>::iterator it = pointDataLabels.begin();
    for (; it != pointDataLabels.end(); it++)
    {
      imageData += "<DataArray Name=\"" + *it + "\" />";
    }
    imageData += "</PointData>";
  }

  imageData += R"(</Piece>
       </UnstructuredGrid>
     </VTKFile>)";

  out.write_attribute<std::string>("vtk.xml", imageData);
  out.write_attribute<std::size_t>("dimension", 3);
  out.write("NumOfVertices", this->numPoints);
  out.write("types", 12); // Type 12: hexahedron, used for 3d shapes
  out.write("NumOfElements", this->numCells);
  out.write("connectivity", this->cells.data(), adios2::Dims(), adios2::Dims(), adios2::Dims{this->numCells,9}); // Hexahedron is 8 points, plus one specifying it will be 8 points

  out.end_step(); // This shouldn't be necessary, but make absolute sure it's flushed
}

template <class T>
adiosApi_impl<T>::~adiosApi_impl()
{
  this->flush();
  this->out.close();
}

template <class T>
void adiosApi<T>::addPoints(const T* x, const T* y, const T* z, size_t count)
{
  this->impl->addPoints(x, y, z, count);
}

template <class T>
void adiosApi_impl<T>::addPoints(const T* x, const T* y, const T* z, size_t count)
{
  if (this->numPoints != 0 )
  {
    std::cerr << "Points may only be added once!" << std::endl;
    return;
  }

  // Paraview takes in a one dimensional array x1, y1, z1, x2, y2, z2, ... xn, yn, zn
  // So we need to interleave the input arrays
  T* points = new T[count*3];
  for (size_t i = 0; i < count; i++)
  {
    points[i*3] = x[i];
    points[i*3+1] = y[i];
    points[i*3+2] = z[i];
  }
  out.write("vertices", points, adios2::Dims(), adios2::Dims(), adios2::Dims{count, 3});

  this->numPoints = count;
}

template <class T>
void adiosApi<T>::addPointData(const std::string &label, const T* data)
{
  this->impl->addPointData(label, data);
}

template <class T>
void adiosApi_impl<T>::addPointData(const std::string &label, const T* data)
{
  // Get the list of points ahead of time so we know how many there are
  if (this->numPoints == 0)
  {
    std::cerr << "Please provide a list of points before trying to add point data" << std::endl;
  }

  this->pointDataLabels.push_back(label); // We need to store this for VTK

  out.write(label, data, adios2::Dims(), adios2::Dims(), adios2::Dims{this->numPoints});
}

template <class T>
void adiosApi<T>::addCell(size_t* connections, size_t count, int offset)
{
  this->impl->addCell(connections, count, offset);
}

template <class T>
void adiosApi_impl<T>::addCell(size_t* connections, size_t count, int offset)
{
  // The Paraview cells list is a 1d array of size_t objects with the required named "connectivity"
  // For each cell, we start off with the number of points and then the list of connected points
  // For example, a connection list "3 1 2 3 4 1 3 4 5" would have two cells composed of points (1, 2, 3) and (1, 3, 4, 5)
  this->cells.push_back(count);
  for (size_t i = 0; i < count; i++)
  {
    cells.push_back(connections[i] - offset);
  }

  this->numCells++;
}  

template class adiosApi<float>;
template class adiosApi_impl<float>;
template class adiosApi<double>;
template class adiosApi_impl<double>;
