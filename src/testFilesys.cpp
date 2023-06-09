//
// Created by 26656 on 2023/6/9.
//
#include "gtest/gtest.h"
#include "global.h"
int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

TEST(testmkdir,should_make_new_dir_when_dir_not_exist_and_if_full_return_false)
{
    EXPECT_EQ( fs->mkdir("testdir1"),true);
    EXPECT_EQ( fs->mkdir("testdir2"),true);
    EXPECT_EQ( fs->mkdir("testdir3"),true);
    EXPECT_EQ( fs->mkdir("testdir4"),true);
    EXPECT_EQ( fs->mkdir("testdir5"),true);
    EXPECT_EQ( fs->mkdir("testdir6"),true);
    EXPECT_EQ( fs->mkdir("testdir7"),true);
    EXPECT_EQ( fs->mkdir("testdir8"),true);
    EXPECT_EQ( fs->mkdir("testdir9"),true);
    EXPECT_EQ( fs->mkdir("testdir10"),true);
    EXPECT_EQ( fs->mkdir("testdir11"),true);
    EXPECT_EQ( fs->mkdir("testdir12"),false);
    EXPECT_EQ( fs->mkdir("testdir13"),false);
    fs->ls("root");
}

TEST(testReadandWrite,should_read_and_write_content){
    fs->touch("testfile1");
    File*file=fs->open("testfile1",1);
    char*content="hello world";
    EXPECT_EQ(fs->write(file,content,strlen(content)),true);
    char*content2=new char[100];
    EXPECT_EQ(fs->read(file,content2),true);
    EXPECT_EQ(strcmp(content,content2),0);
    std::cout<<content2<<std::endl;
}

TEST(testBigDataReadandWrite,testReadandWrite_should_read_and_write_content_Test){
    fs->touch("testfile2");
    File*file=fs->open("testfile2",1);
    char*content=new char[16*25];
    for(int i=0;i<16*25;i++){
        if(i<=16*11)
            content[i]='a';
        else content[i]='b';
    }
    EXPECT_EQ(fs->write(file,content,strlen(content)),true);
    char*content2=new char[16*25+1];
    EXPECT_EQ(fs->read(file,content2),true);
    EXPECT_EQ(strcmp(content,content2),0);
    std:cout<<"读出了"<<strlen(content2)<<"个字符"<<std::endl;
    std::cout<<content2<<std::endl;
}

TEST(testDeleteFileorDir,should_not_get_the_file_or_dir_if_been_del){
    fs->touch("testfile1");
    fs->mkdir("testdir1");
    std::cout<<"删除前："<<std::endl;
    fs->ls("root");//查看当前目录
    fs->cd("testdir1");//进入目录
    fs->touch("testfile2");//创建文件
    fs->cd("..");//返回上一级目录
    EXPECT_EQ(fs->rm("testdir1"),true);
    EXPECT_EQ(fs->rm("testfile1"),true);
    std::cout<<"删除后："<<std::endl;
    fs->ls("root");//查看当前目录
}