pipeline {
    agent any
    stages {
        stage('Configure'){
            steps {
                dir('build') {
                    sh 'cmake ../'
                }
            }

        }
        stage('Build'){
            steps {
                dir('build'){
                    sh 'make'
                }
            }
        }
        stage('Test'){
            steps {
                dir('build') {
                    sh 'ctest -T test --no-compress-output'
                }
             }
        }
        stage('Deploy'){
            steps {
                sh 'git clone git://github.com/mosra/m.css'
                sh './m.css/documentation/doxygen.py Doxyfile-mcss'
            }
        }
    }
    post {
        always {
            // Archive the CTest xml output
            archiveArtifacts(
                artifacts: 'build/Testing/**/*.xml',
                fingerprint: true
            )
            // Process the CTest xml out with XUnit Plugin
            xunit([CTest(deleteOutputFiles: true, failIfNotNew: true, pattern: 'build/Testing/**/*.xml', skipNoTestFiles: false, stopProcessingIfError: true)])
            deleteDir()
        }
    }

}
