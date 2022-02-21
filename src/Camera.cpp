#include <Camera.h>

ParkingArrowCamera::ParkingArrowCamera()
{
    cm = std::make_unique<libcamera::CameraManager>();
    cm->start();
    if (cm->cameras().empty())
    {
        cm->stop();
        return;
    }
    std::string cameraId = cm->cameras()[0]->id();
    camera = cm->get(cameraId);
    camera->acquire();
    std::unique_ptr<libcamera::CameraConfiguration> config =
        camera->generateConfiguration({libcamera::StreamRole::VideoRecording}); 
    config->at(0).pixelFormat = libcamera::formats::MJPEG;
    config->at(0).size.width = Y_PLANE_WIDTH;
    config->at(0).size.height = Y_PLANE_HEIGHT;
    config->at(0).stride= Y_PLANE_WIDTH;
    config->at(0).frameSize = FRAME_SIZE;
    config->at(0).bufferCount = 1;
    config->validate();
    camera->configure(config.get());
    allocator = new libcamera::FrameBufferAllocator(camera);
    for (libcamera::StreamConfiguration &cfg : *config)
    {
        int ret = allocator->allocate(cfg.stream());
        if (ret < 0)
        {
            return;
        }
        size_t allocated = allocator->buffers(cfg.stream()).size();
    }

    stream = config->at(0).stream();
    const std::vector<std::unique_ptr<libcamera::FrameBuffer>> &buffers = allocator->buffers(stream);
    for (unsigned int i = 0; i < buffers.size(); ++i)
    {
        std::unique_ptr<libcamera::Request> request = camera->createRequest();
        if (!request)
        {
            return;
        }
        const std::unique_ptr<libcamera::FrameBuffer> &buffer = buffers[i];
        int ret = request->addBuffer(stream, buffer.get());
        if (ret < 0)
        {
            return;
        }
        libcamera::ControlList &controls = request->controls();
        requests.push_back(std::move(request));
    }
    new_frame_received = false;
    frame = new cv::Mat(Y_PLANE_HEIGHT, Y_PLANE_WIDTH, CV_8UC3);
    camera->requestCompleted.connect(this, &ParkingArrowCamera::requestComplete);
}

bool ParkingArrowCamera::get_next_frame(cv::Mat *f)
{
    if (new_frame_received)
    {
        new_frame_received = false;
        frame->copyTo(*f);
        return true;
    }
    return false;
}

void ParkingArrowCamera::start()
{
    camera->start();
    for (std::unique_ptr<libcamera::Request> &request : requests)
        camera->queueRequest(request.get());
}

ParkingArrowCamera::~ParkingArrowCamera()
{
    camera->stop();
    allocator->free(stream);
    delete allocator;
    camera->release();
    camera.reset();
    cm->stop();
}

void ParkingArrowCamera::requestComplete(libcamera::Request *request)
{
    if (request->status() == libcamera::Request::RequestCancelled)
    {
        return;
    }
    const libcamera::Request::BufferMap &buffers = request->buffers();
    for (auto bufferPair : buffers)
    {
        libcamera::FrameBuffer *buffer = bufferPair.second;
        const libcamera::FrameMetadata &metadata = buffer->metadata();
        const libcamera::FrameBuffer::Plane &Yplane = buffer->planes()[0];
        const libcamera::FrameBuffer::Plane &Uplane = buffer->planes()[1];
		void *Ymemory = mmap(NULL, Yplane.length + Uplane.length, PROT_READ | PROT_WRITE, MAP_SHARED, Yplane.fd.get(), 0);
        cv::Mat YpicNV12 = cv::Mat(Y_PLANE_HEIGHT, Y_PLANE_WIDTH, CV_8UC1, (void*)Ymemory);
        cv::Mat UpicNV12 = cv::Mat(U_PLANE_HEIGHT, U_PLANE_WIDTH, CV_8UC2, (void*)Ymemory + Yplane.length);
        cv::cvtColorTwoPlane(YpicNV12, UpicNV12, *frame, cv::COLOR_YUV2BGR_NV12);
        new_frame_received = true;
    }
    request->reuse(libcamera::Request::ReuseBuffers);
    camera->queueRequest(request);
}