#include "modelmodel.h"

using namespace Endoscope;

ModelModel::ModelModel(QObject* parent) : ObjectModelBase<QAbstractItemModel>( parent )
{
}

QVariant ModelModel::data(const QModelIndex& index, int role) const
{
  if ( !index.isValid() )
    return QVariant();

  QAbstractItemModel* model = static_cast<QAbstractItemModel*>( index.internalPointer() );
  if ( !model )
    return QVariant();
  return dataForObject( model, index, role );
}

int ModelModel::rowCount(const QModelIndex& parent) const
{
  if ( parent.isValid() ) {
    QAbstractItemModel* sourceModel = static_cast<QAbstractItemModel*>( parent.internalPointer() );
    Q_ASSERT( sourceModel );
    const QVector<QAbstractProxyModel*>  proxies = proxiesForModel( sourceModel );
    return proxies.size();
  }

  return m_models.size();
}

QModelIndex ModelModel::parent(const QModelIndex& child) const
{
  QAbstractItemModel* model = static_cast<QAbstractItemModel*>( child.internalPointer() );
  Q_ASSERT( model );
  if ( m_models.contains( model ) )
    return QModelIndex();

  QAbstractProxyModel* proxy = qobject_cast<QAbstractProxyModel*>( model );
  Q_ASSERT( proxy );
  return indexForModel( proxy->sourceModel() );
}

QModelIndex ModelModel::index(int row, int column, const QModelIndex& parent) const
{
  if ( parent.isValid() ) {
    QAbstractItemModel *model = static_cast<QAbstractItemModel*>( parent.internalPointer() );
    const QVector<QAbstractProxyModel*> proxies = proxiesForModel( model );
    if ( proxies.size() <= row )
      return QModelIndex();
    return createIndex( row, column, proxies.at( row ) );
  }
  return createIndex( row, column, m_models.at( row ) );
}

void ModelModel::objectAdded(const QPointer< QObject >& obj)
{
  QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel*>( obj );
  if ( proxy ) {
    beginResetModel(); // FIXME
    m_proxies.push_back( proxy );
    endResetModel();
    return;
  }

  QAbstractItemModel *model = qobject_cast<QAbstractItemModel*>( obj );
  if ( model ) {
    beginInsertRows( QModelIndex(), m_models.size(), m_models.size() );
    m_models.push_back( model );
    endInsertRows();
  }
}

void ModelModel::objectRemoved(QObject* obj)
{
  int index = m_models.indexOf( static_cast<QAbstractItemModel*>( obj ) );
  if ( index >= 0 && index < m_models.size() ) {
    beginRemoveRows( QModelIndex(), index, index );
    m_models.remove( index );
    endRemoveRows();
  }

  for ( QVector<QAbstractProxyModel*>::iterator it = m_proxies.begin(); it != m_proxies.end(); ++it ) {
    if ( *it == obj ) {
      beginResetModel(); // FIXME
      m_proxies.erase( it );
      endResetModel();
      return;
    }
  }
}

QModelIndex ModelModel::indexForModel(QAbstractItemModel* model) const
{
  if ( !model )
    return QModelIndex();
  QAbstractProxyModel* proxy = qobject_cast<QAbstractProxyModel*>( model );
  if ( !proxy ) {
    Q_ASSERT( m_models.contains( model ) );
    return index( m_models.indexOf( model ), 0, QModelIndex() );
  }

  QAbstractItemModel *sourceModel = proxy->sourceModel();
  const QModelIndex parentIndex = indexForModel( sourceModel );
  const QVector<QAbstractProxyModel*> proxies = proxiesForModel( sourceModel );
  Q_ASSERT( proxies.contains( proxy ) );
  return index( proxies.indexOf( proxy ), 0, parentIndex );
}

QVector<QAbstractProxyModel*> ModelModel::proxiesForModel(QAbstractItemModel* model) const
{
  QVector<QAbstractProxyModel*> proxies;
  if ( !model )
    return proxies;

  foreach ( QAbstractProxyModel* proxy, m_proxies ) {
    if ( proxy && proxy->sourceModel() == model )
      proxies.push_back( proxy );
  }

  return proxies;
}

#include "modelmodel.moc"