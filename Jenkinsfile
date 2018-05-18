pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
      additionalBuildArgs '--pull'
      reuseNode true
    }

  environment {
    AGENT_INFO = ''
  }

  }
  stages {
    stage('openmpi module') {
      steps {
        sh '''
             source /etc/profile.d/modules.sh
             module load mpi/openmpi-x86_64
             if [ ! -d "build" ]; then
                mkdir build;
             fi
             cd build
             cmake ..
             make all -j4
             ctest '''
      }
    }
  }
}
