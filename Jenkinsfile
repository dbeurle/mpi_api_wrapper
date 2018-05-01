pipeline {
  agent none
  stages {
    stage('build') {
      agent {
        dockerfile {
          filename 'Dockerfile.build'
        }

      }
      steps {
        sh 'mkdir build && cd build && source /etc/profile.d/modules.sh && module load mpi/openmpi-x86_64  && cmake .. && make all -j4'
      }
    }
    stage('test') {
      steps {
        sh 'cd build && ctest'
      }
    }
  }
}