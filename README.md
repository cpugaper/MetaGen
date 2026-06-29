# MetaGen: Genetic Inheritance System for MetaHumans

<p align="center">
  <img width="1128" height="643" alt="portada" src="https://github.com/user-attachments/assets/03fd1bf0-1bbe-4bad-9dbe-fc64702ec7a3" />
</p>

## Introduction

MetaGen is a tool developed for Unreal Engine 5 as a Bachelor's Thesis (TFG). It provides a streamlined, time-saving workflow for technical artists and developers to automatically generate families using Epic Games' MetaHumans.

As an initial prototype, the core architecture focuses on runtime generation rather than inside the Unreal Editor viewports. By using real-time Morph Targets and Data Tables, MetaGen computes mendelian genetic inheritance and morphological interpolation to generate multiple children from two selected MetaHuman parents, while still allowing final user customization.

The project serves as an interactive simulation template, featuring 6 pre-configured parent models and 1 child base model. 

---

## Developer Guide: Integrating New Parent Characters

To extend the system and add your own custom parental models, access the production pipeline template and detailed documentation [here](https://drive.google.com/drive/folders/1hj6qT3SajAVpVFESHT1llWZMrvKYUuVQ?dmr=1&ec=wgc-drive-globalnav-goto).

The package includes the source production file (`GeneticChild_Template.blend`) and the full step-by-step documentation covering the following implementation pipeline:

#### 1. Mendelian Trait Configuration (Unreal Engine 5)
Registering basic genetic data within the `DT_Parents` Data Table.

#### 2. Facial Geometry Extraction (Unreal Engine 5)
Using the MetaHuman Identity component to track facial features and export normalized `.OBJ` facial meshes to maintain vertex topology across new models.

#### 3. Morph Target Generation and Segmentation (Blender)
Importing parental meshes into the base template, transferring facial proportions to the base child mesh via shape keys, and isolating individual facial features using vertex groups.

#### 4. Mutable System Integration (Unreal Engine 5)
Importing the compiled `.FBX` skeletal mesh, linking the asset inside the `CO_Child_Genetics` Mutable object graph, enabling Real Time Morph Targets, and synchronizing references.

---

## Development Showcase

You can watch a brief behind-the-scenes video showcasing the development process of the project below. [![Watch the MetaGen Development Showcase](https://img.youtube.com/vi/J1XMF349okU/maxresdefault.jpg)](https://www.youtube.com/watch?v=J1XMF349okU)

---
Developed by Carla Puga Pérez as a Bachelor's Thesis.

>**_NOTE:_** Asset persistent saving or exporting runtime mesh modifications back to editor assets falls outside the scope of this prototype.
