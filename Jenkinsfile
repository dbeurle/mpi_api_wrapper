pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
      additionalBuildArgs '--pull'
      reuseNode true
    }

  }
  stages {
    stage('openmpi gcc debug') {
      steps {
        sh '''
             source /etc/profile.d/modules.sh
             module load mpi/openmpi-x86_64
             if [ ! -d "build" ]; then
                mkdir build;
             fi
             cd build
             cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=1 ..
             make all
             ctest
           '''
      }
      post{
          success{
              sh '''
                  cd build
                  ls -R
                  make coverage
              '''
          }
      }
    }
    stage('openmpi gcc release') {
      steps {
        sh '''
             source /etc/profile.d/modules.sh
             module load mpi/openmpi-x86_64
             if [ ! -d "build" ]; then
                mkdir build;
             fi
             cd build
             cmake -DCMAKE_BUILD_TYPE=Release ..
             make all -j4
             ctest
           '''
      }
    }
  }
}
