pipeline {
  agent {
    docker {
      image 'fedora:latest'
    }

  }
  stages {
    stage('build') {
      agent any
      steps {
        sh 'ls; pwd; docker ps -a; mkdir build ; cd build && source /etc/profile.d/modules.sh && module load mpi/openmpi-x86_64  && cmake .. && make all -j4'
      }
    }
    stage('test') {
      steps {
        sh 'cd build && ctest'
      }
    }
  }
}