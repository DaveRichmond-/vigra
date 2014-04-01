import vigra
import vigra.graphs as vigraph

f       = '100075.jpg'
sigma   = 3.0

img                 = vigra.impex.readImage(f)
imgLab              = vigra.colors.transform_RGB2Lab(img)
imgLabInterpolated  = vigra.resize(imgLab, [imgLab.shape[0]*2-1, imgLab.shape[1]*2-1])
gradmagInterpolated = vigra.filters.gaussianGradientMagnitude(imgLabInterpolated, sigma)
labels,nseg = vigra.analysis.slicSuperpixels(imgLab, 10.0, 5)
labels  = vigra.analysis.labelImage(labels)


graph0,graph1 = vigraph.gridRegionAdjacencyGraph(labels=labels,ignoreLabel=None )

# get grid graph and edge weights
graph0EdgeWeights = vigraph.edgeFeaturesFromInterpolatedImage(graph0, gradmagInterpolated)
graph1EdgeWeights = graph1.accumulateEdgeFeatures(graph0EdgeWeights, acc='mean')
graph1NodeFeatures = graph1.accumulateNodeFeatures(img, acc='mean')
graph1Labels = vigraph.felzenszwalbSegmentation(graph1, graph1EdgeWeights, k=10, nodeNumStop=1000)


graph2       = vigraph.regionAdjacencyGraph(graph=graph1, labels=graph1Labels, ignoreLabel=None)
graph2EdgeWeights = graph2.accumulateEdgeFeatures(graph1EdgeWeights,acc='mean')
graph2Labels = vigraph.felzenszwalbSegmentation(graph2,graph2EdgeWeights, k=20, nodeNumStop=200)
graph3 = vigraph.regionAdjacencyGraph(graph=graph2, labels=graph2Labels, ignoreLabel=None)



graphs = [graph1, graph2, graph3]

for g in graphs:
    print g.nodeNum
    g.show(img=img)
    vigra.show()

