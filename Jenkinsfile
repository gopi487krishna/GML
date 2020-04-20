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
                sh 'git clone git://github.com/gopi487krishna/gml-docs'
                sh './m.css/documentation/doxygen.py Doxyfile-mcss'
                sh 'rsync -a -delete html/ gml-docs/'
                withCredentials([usernamePassword(credentialsId: '3122', passwordVariable: 'GIT_PASSWORD', usernameVariable: 'GIT_USERNAME')]) {
                     sh ("echo Starting to Publish")
                     sh("git tag -a some_tag -m 'Jenkins'")
                     sh('git push https://${GIT_USERNAME}:${GIT_PASSWORD}@gml-docs --tags')
                }
            }
        }
       
        
    }
    post {
        always {
            
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
