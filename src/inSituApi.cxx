#include "inSituApi.h"

#include <adios2.h>
#include <mpi.h>

template <class T>
class inSituApi_impl {

  adios2::fstream out;
  std::vector<std::string> labels;

  public:
    inSituApi_impl(std::string fn="inSitu-writer");
    ~inSituApi_impl();

    void flush();

    void addPoints(const T*, const T*, const T*, int, int, int);
    void addPointData(const std::string &, const T*);
    void addCells(size_t*, int, int, int, int);

  private:
    int numPoints = 0;
    int pointStart = 0;
    int globalPoints = 0;
    int numCells = 0;
    std::vector<std::string> pointDataLabels;
};

template <class T>
inSituApi<T>::inSituApi(std::string fn)
{
  impl = new inSituApi_impl<T>(fn);
}

template <class T>
inSituApi_impl<T>::inSituApi_impl(std::string fn)
{
  out = adios2::fstream(fn, adios2::fstream::out, "sst");
}

template <class T>
inSituApi<T>::~inSituApi()
{
  delete impl;
}

template <class T>
void inSituApi_impl<T>::flush()
{
  if (this->numPoints == 0 || this->numCells == 0)
  {
    std::cerr << "Nothing to flush" << std::endl;
  }

  out.end_step(); // This is necessary for a streaming inSitu type to work
}

template <class T>
inSituApi_impl<T>::~inSituApi_impl()
{
  if (this->numPoints != 0 && this->numCells != 0)
  {
    this->flush();
  }
  this->out.close();
}

template <class T>
void inSituApi<T>::addPoints(const T* x, const T* y, const T* z, int count, int start, int size)
{
  this->impl->addPoints(x, y, z, count, start, size);
}

template <class T>
void inSituApi_impl<T>::addPoints(const T* x, const T* y, const T* z, int count, int start, int size)
{
  if (this->numPoints != 0 )
  {
    std::cerr << "Points may only be added once!" << std::endl;
    return;
  }

  out.write("x", x, adios2::Dims{count}, adios2::Dims{start}, adios2::Dims{size});

  this->numPoints = count;
  this->pointStart = start;
  this->globalPoints = size;
}

template <class T>
void inSituApi<T>::addPointData(const std::string &label, const T* data)
{
  this->impl->addPointData(label, data);
}

template <class T>
void inSituApi_impl<T>::addPointData(const std::string &label, const T* data)
{
  // Get the list of points ahead of time so we know how many there are
  if (this->numPoints == 0)
  {
    std::cerr << "Please provide a list of points before trying to add point data" << std::endl;
  }

  this->pointDataLabels.push_back(label); // We need to store this for VTK

  // Shape/start/size should ALWAYS be the same for points/pointdata, so reuse old info
  out.write(label, data, adios2::Dims{this->numPoints}, adios2::Dims{this->pointStart}, adios2::Dims{this->globalPoints});
}

template <class T>
void inSituApi<T>::addCells(size_t* connections, int cellSize, int count, int start, int size)
{
  this->impl->addCells(connections, cellSize, count, start, size);
}

template <class T>
void inSituApi_impl<T>::addCells(size_t* connections, int cellSize, int count, int start, int size)
{
  // This interface is changing, cellSize isn't currently used and numCells isn't used except to make sure we've added some cells
  out.write("cells", connections, adios2::Dims{count}, adios2::Dims{start}, adios2::Dims{size});
  this->numCells = count;
}  

template class inSituApi<float>;
template class inSituApi_impl<float>;
template class inSituApi<double>;
template class inSituApi_impl<double>;
