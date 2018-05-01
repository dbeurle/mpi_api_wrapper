pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
    }

  }
  stages {
    stage('build') {
      agent {
        dockerfile {
          filename 'Dockerfile.build'
        }

      }
      steps {
        sh 'mkdir build && cd build && which module && module load mpi/openmpi-x86_64  && cmake .. && make all -j4'
      }
    }
  }
}