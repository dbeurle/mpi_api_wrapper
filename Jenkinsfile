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
             make all -j3
             ctest
           '''
      }
      post{
          success{
              sh '''
                  cd build
                  make coverage
                  gcovr --xml --output coverage.xml
                  cobertura autoUpdateHealth: false,
                  autoUpdateStability: false,
                  coberturaReportFile: 'build/coverage.xml',
                  conditionalCoverageTargets: '70, 79, 80',
                  failUnhealthy: false,
                  failUnstable: false,
                  lineCoverageTargets: '80, 79, 80',
                  maxNumberOfBuilds: 0,
                  methodCoverageTargets: '80, 79, 80',
                  onlyStable: false,
                  sourceEncoding: 'ASCII',
                  zoomCoverageChart: false
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
             make all -j3
             ctest
           '''
      }
    }
  }
}
