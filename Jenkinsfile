pipeline {
  agent {
    docker {
      image 'fedora:latest'
    }
    
  }
  stages {
    stage('print_garbage') {
      steps {
        sh 'echo "Hello world"'
      }
    }
  }
}