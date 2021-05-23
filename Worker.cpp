#include "Worker.hpp"

#include "IWork.hpp"
#include "Cluster.hpp"

Worker::Worker(Cluster *cluster) {
  work_ = 0;
  cluster_ = cluster;
  keepOn = true;
}

Worker::~Worker() {
  delete this->work_;
  this->finish();
}

void Worker::_setWork(IWork *work) {
  delete work_;
  work_ = work;
}

void Worker::getWork() {
  _setWork(cluster_->getWork());
}

void Worker::doWork() {
  work_->doWork(cluster_);
}

void Worker::run() {
  ::pthread_create(&pthread_, 0, _curcle, this);
}

void Worker::finish() {
  keepOn = false;
  ::pthread_join(pthread_, 0);
}

void *Worker::_curcle(void *arg) {
  Worker *self = static_cast<Worker *>(arg);
  while (self->isKeepOn()) {
    self->getWork();
    self->cluster_->incActiveWorkers();
    self->doWork();
    self->cluster_->decActiveWorkers();
  }
  return 0;
}

bool Worker::isKeepOn() const {
  return keepOn;
}
