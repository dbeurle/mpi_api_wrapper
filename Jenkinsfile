pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
    }

  }
  stages {
    stage('build') {
      steps {
        sh 'mkdir build && cd build && module load mpi/openmpi-x86_64  && cmake .. && make all -j4'
      }
    }
  }
}