#include "byte_stream.hh"

using namespace std;
// : capacity_(capacity) This is the constructor initializer list. 
// It initializes member variables of the ByteStream class.
ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), available_capacity_( capacity ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return is_closed_;
}

void Writer::push( string data )
{
  // Your code here.
  auto len = min ( available_capacity_, data.size() );
  if( len < data.size() ) data = data.substr(0, len);
  if( len > 0) {
    available_capacity_ -= len;
    bytes_pushed_ += len;
    bytes_buffered_ += len;
    // move(data) is a way to transfer ownership of the data held by data into the vector without making a copy. 
    // It's used here assuming that data is a moveable object, typically achieved by using std::move.
    buffer_.emplace_back( move(data) );
    // buffer_.back() retrieves a reference to the last element added to buffer_
    buffer_view_.emplace_back( buffer_.back() );
  }
}

void Writer::close()
{
  // Your code here.
  is_closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return available_capacity_;
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return is_closed_ && bytes_buffered_ == 0;
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_popped_;
}

string_view Reader::peek() const
{
  // Your code here.
  // deque.front()
  return buffer_view_.front();
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  len = min (bytes_buffered_, len);
  auto sz = len;
  while( len > 0) {
    auto size_front = buffer_view_.front().size();
    if(len >= size_front) buffer_view_.pop_front();
    else {
      buffer_view_.front().remove_prefix( len );
      break;
    }
    len -= size_front; 
  }
  available_capacity_ += sz;
  bytes_popped_ += sz;
  bytes_buffered_ -= sz;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return bytes_buffered_;
}
