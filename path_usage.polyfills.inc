<?php

/**
 * Implements hook_TYPE_path_usages() for the node module.
 */
function system_path_usages($entity, $object_type) {
  // entity uri only applies to entities (but to all entities), so let's
  // ignore anything that's not an entity here.
  $types = array_keys(entity_get_info());
  if (!in_array($object_type, $types)) {
    return array();
  }
  $paths = array();
  $uri = entity_uri($object_type, $entity);
  if (!empty($uri)) {
    $paths[] = $uri['path'];
  }
  return $paths;
}

/**
 * Implements hook_view_path_usages() for views.
 */
function views_view_path_usages($view, $display_id) {
  $paths = array();
  $view->set_display($display_id);
  $path = $view->get_path();
  if (!empty($path)) {
    $paths[] = $path;
  }
  return $paths;
}

/**
 * Implements hook_path_usages() for views.
 */
/*
function views_path_usages($entity, $object_type) {
  // views only applies to entities (but to all entities), so let's
  // ignore anything that's not an entity here.
  $info = entity_get_info($object_type);
  if (empty($info)) { return array(); }
  $views = views_get_all_views();
  foreach ($views as $view) {
    if (!$view->disabled) {
      if ($view->base_table == $info['base table']) {
        foreach ($view->display as $id => $display) {
          $view->display[$id]->display_options['filters']['nid_1'] = array(
            'id' => 'nid_1',
            'table' => $info['base table'],
            'field' => $info['entity keys']['id'],
            'value' => $entity->{$info['entity keys']['id']},
          );
          $view->execute($id);
          if (count($view->result)) {
path_usage_view_paths($view, $id);
            $paths += 
          }
        }
      }
    }
  }
}
*/

/**
 * Implements hook_TYPE_path_
 */
function entityreference_path_usages($node, $object_type) {
  // entity reference only applies to entities (but to all entities), so let's
  // ignore anything that's not an entity here.
  $types = array_keys(entity_get_info());
  if (!in_array($object_type, $types)) {
    return array();
  }
  $paths = array();
  $er_fields = _path_usage_find_fields_of_type('entityreference');
  module_load_include('inc', 'path_usage');
  foreach ($er_fields as $field) {
    if ($field['settings']['target_type'] == 'node' &&
        in_array($node->type, $field['settings']['handler_settings']['target_bundles'])) {
      $entities = _path_usage_find_all_entities();
      foreach ($entities as $entity_type => $entities) {
        foreach ($entities as $entity) {
          // we want to include all entities that reference this entity and
          // all things that use that entity as well, so we recurse.
          $paths += path_usage_entity_paths($entity_type, $entity);
        }
      }
    }
  }
  return $paths;
}

function taxonomy_path_usages($entity, $object_type, $subtype = NULL) {
  // term references only applies to entities (but to all entities), so let's
  // ignore anything that's not an entity here.
  $types = array_keys(entity_get_info());
  if (!in_array($object_type, $types)) {
    return array();
  }
  $paths = array();
  $terms = array();
  $languages = array_keys(language_list());
  $fields = _path_usage_find_fields_of_type('taxonomy_term_reference', $object_type, $subtype);
  foreach ($fields as $field) {
    $field_name = $field['field_name'];
    foreach ($languages as $language) {
      if (!empty($entity->{$field_name}[$language]) &&
          is_array($entity->{$field_name}[$language]) &&
          !empty($entity->{$field_name}[$language[0]['tid']])) {
        foreach ($entity->{$field_name}[$language] as $data) {
          $terms[$data['tid']] = $data['tid'];
        }
      }
    }
  }
  foreach ($terms as $tid) {
    $term = taxonomy_term_load($tid);
    $paths[] = entity_uri('taxonomy_term', $term);
  }
  return $paths;
}

/**
 * Implements hook_path_usages_alter() for the path module.
 */
function path_path_usages_alter(&$paths) {
  $languages = language_list();
  $aliases = array();
  foreach ($languages as $language) {
    foreach ($paths as $path) {
      $alias = url($path, array(
        'language' => $language,
        'absolute' => TRUE,
      ));
      $aliases[] = $alias;
    }
  }
  var_dump($aliases);
  $paths = array_merge($paths, $aliases);
}