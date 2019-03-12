# Forward_Project

## Table of Contents
   * [Introduction](#introduction)
   * [Dataset Description](#dataset-description)

## Introduction
In this project, we present the DESK (Dexterous Surgical Skill) dataset. It comprises a set of surgical robotic skills collected during a surgical training task using three robotic platforms: the Taurus II robot, Taurus II simulated robot, and the YuMi robot. This dataset was used to test the idea of transferring knowledge across different domains (e.g. from Taurus to YuMi robot) for a surgical gesture classification task with seven gestures. We explored three different scenarios: 1) No transfer, 2) Transfer from simulated Taurus to real Taurus and 3) Transfer from Simulated Taurus to the YuMi robot. We conducted extensive experiments with three supervised learning models and provided baselines in each of these scenarios. Results show that using simulation data during training enhances the performance on the real robot where limited real data is available. In particular, we obtained an accuracy of 55% on the real Taurus data using a model that is trained only on the simulator data. Furthermore, we achieved an accuracy improvement of 34% when 3% of the real data is added into the training process.

Link to the arxiv submission: [paper](https://arxiv.org/abs/1903.00959)
Link to the dataset: [data](https://purdue0-my.sharepoint.com/personal/gonza337_purdue_edu/_layouts/15/onedrive.aspx?FolderCTID=0x0120004A851C9347F497499945A81A5DCD69F0&id=%2Fpersonal%2Fgonza337_purdue_edu%2FDocuments%2FForward%2FData%20Collection)

## Dataset Description
Peg transfer task was performed by each of the three robots by a group of human subjects. Each peg transfer consists of a set seven surgemes and each surgeme was marked as a success or failure. The surgemes are annotated using a annotation tool developed by us. Furthermore, we also provide the data related to the segmentation of the objects on the peg board.

### Taurus robot
### Taurus simulator
### YuMi robot
