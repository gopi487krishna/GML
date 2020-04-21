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
        stage("Publish") {
            steps {
                sh 'git clone git://github.com/mosra/m.css'
                sh 'git clone git://github.com/gopi487krishna/gml-docs'
                sh './m.css/documentation/doxygen.py Doxyfile-mcss'
                sh 'rsync -a -delete html/ gml-docs/docs'
                 withCredentials([usernamePassword(credentialsId: '3122', passwordVariable: 'GIT_PASSWORD', usernameVariable: 'GIT_USERNAME')]) {
                     dir('gml-docs'){
                     sh ("git add .")
                     script {
                         REPO_STATUS = sh (
                         script: "git commit -m 'Updated Documentation' ",
                         returnStatus:true
                         )
                     }
                     sh ('echo ${REPO_STATUS}')
                     //when{expression{REPO_STATUS ==true}}
                     //steps{
                     //sh('git push https://${GIT_USERNAME}:${GIT_PASSWORD}@github.com/${GIT_USERNAME}/gml-docs --all')
                     //}
                     }
                     
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


